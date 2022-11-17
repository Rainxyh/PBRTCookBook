#include "RenderThread.h"
#include "DebugText.hpp"
#include <QElapsedTimer>

#include "Core/FeimosRender.h"
#include "Core/primitive.h"
#include "Core/Spectrum.h"
#include "Core/interaction.h"
#include "Core/Scene.h"

#include "Shape/Triangle.h"
#include "Shape/plyRead.h"

#include "Accelerator/BVHAccel.h"

#include "Camera/Camera.h"
#include "Camera/Perspective.h"

#include "Sampler/Sampler.h"
#include "Sampler/clockRand.h"

#include "Integrator/Integrator.h"

#include "RenderStatus.h"

RenderThread::RenderThread()
{
	paintFlag = false;
	renderFlag = false;
}

RenderThread::~RenderThread()
{
}

void RenderThread::run()
{
	emit PrintString("Prepared to Render");

	ClockRandomInit();

	int WIDTH = 500;
	int HEIGHT = 500;

	emit PrintString("Init FrameBuffer");
	p_framebuffer->bufferResize(WIDTH, HEIGHT);

	emit PrintString("Init Camera");
	std::shared_ptr<Feimos::Camera> camera;
	//初始化程序
	Feimos::Point3f eye(-3.0f, 1.5f, -3.0f), look(0.0, 0.0, 0.0f);
	Feimos::Vector3f up(0.0f, 1.0f, 0.0f);
	Feimos::Transform lookat = LookAt(eye, look, up);
	//取逆是因为LookAt返回的是世界坐标到相机坐标系的变换
	//而我们需要相机坐标系到世界坐标系的变换
	Feimos::Transform Camera2World = Inverse(lookat);
	camera = std::shared_ptr<Feimos::Camera>(Feimos::CreatePerspectiveCamera(WIDTH, HEIGHT, Camera2World));

	// 生成Mesh加速结构
	std::shared_ptr<Feimos::TriangleMesh> mesh;
	std::vector<std::shared_ptr<Feimos::Shape>> tris;
	std::vector<std::shared_ptr<Feimos::Primitive>> prims;
	Feimos::plyInfo *plyi;
	std::shared_ptr<Feimos::Aggregate> aggregate;
	Feimos::Transform tri_Object2World, tri_World2Object;

	tri_Object2World = Feimos::Translate(Feimos::Vector3f(0.0, -2.5, 0.0)) * tri_Object2World;
	tri_World2Object = Inverse(tri_Object2World);
	emit PrintString("Read Mesh");
	plyi = new Feimos::plyInfo("../../Resources/dragon.3d");
	mesh = std::make_shared<Feimos::TriangleMesh>(tri_Object2World, plyi->nTriangles, plyi->vertexIndices, plyi->nVertices, plyi->vertexArray, nullptr, nullptr, nullptr, nullptr);
	tris.reserve(plyi->nTriangles);
	emit PrintString("Init Triangles");
	for (int i = 0; i < plyi->nTriangles; ++i)
		tris.push_back(std::make_shared<Feimos::Triangle>(&tri_Object2World, &tri_World2Object, false, mesh, i));
	emit PrintString("Init Primitives");
	for (int i = 0; i < plyi->nTriangles; ++i)
		prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(tris[i]));
	emit PrintString("Init Accelerator");
	aggregate = std::make_unique<Feimos::BVHAccel>(prims, 1);

	emit PrintString("Init Sampler");
	Feimos::Bounds2i imageBound(Feimos::Point2i(0, 0), Feimos::Point2i(WIDTH, HEIGHT));
	std::shared_ptr<Feimos::ClockRandSampler> sampler = std::make_unique<Feimos::ClockRandSampler>(8, imageBound);

	std::unique_ptr<Feimos::Scene> worldScene = std::make_unique<Feimos::Scene>(aggregate);
	Feimos::Bounds2i ScreenBound(Feimos::Point2i(0, 0), Feimos::Point2i(WIDTH, HEIGHT));

	std::shared_ptr<Feimos::Integrator> integrator = std::make_shared<Feimos::SamplerIntegrator>(camera, sampler, ScreenBound, p_framebuffer);

	emit PrintString("Start Rendering");
	// 开始执行渲染
	int renderCount = 0;
	while (renderFlag)
	{
		QElapsedTimer t;
		t.start();

		double frameTime;
		integrator->Render(*worldScene, frameTime);

		m_RenderStatus.setDataChanged("Performance", "One Frame Time", QString::number(frameTime), "");
		m_RenderStatus.setDataChanged("Performance", "Frame pre second", QString::number(1.0f / (float)frameTime), "");

		emit PaintBuffer(p_framebuffer->getUCbuffer(), WIDTH, HEIGHT, 4);

		while (t.elapsed() < 1)
			;
	}

	emit PrintString("End Rendering");
}
