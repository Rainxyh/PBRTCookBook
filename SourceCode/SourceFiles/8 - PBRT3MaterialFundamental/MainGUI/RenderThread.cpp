#include "RenderThread.h"
#include "DebugText.hpp"
#include <QElapsedTimer>

#include "Core/FeimosRender.h"
#include "Core/primitive.h"
#include "Core/Spectrum.h"
#include "Core/interaction.h"
#include "Core/Scene.h"
#include "Core/Transform.h"

#include "Shape/Triangle.h"
#include "Shape/plyRead.h"

#include "Accelerator/BVHAccel.h"

#include "Camera/Camera.h"
#include "Camera/Perspective.h"

#include "Sampler/Sampler.h"
#include "Sampler/clockRand.h"

#include "Integrator/Integrator.h"

#include "Material/Material.h"
#include "Material/MatteMaterial.h"

#include "Texture/Texture.h"
#include "Texture/ConstantTexture.h"

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
	Feimos::Transform Camera2World = Feimos::Inverse(lookat);
	camera = std::shared_ptr<Feimos::Camera>(Feimos::CreatePerspectiveCamera(WIDTH, HEIGHT, Camera2World));

	// 生成材质与纹理
	emit PrintString("Init Material");
	Feimos::Spectrum floorColor;
	floorColor[0] = 0.2;
	floorColor[1] = 0.3;
	floorColor[2] = 0.9;
	Feimos::Spectrum dragonColor;
	dragonColor[0] = 1.0;
	dragonColor[1] = 1.0;
	dragonColor[2] = 0.0;
	std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> KdDragon = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(dragonColor);
	std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> KdFloor = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(floorColor);
	std::shared_ptr<Feimos::Texture<float>> sigma = std::make_shared<Feimos::ConstantTexture<float>>(0.0f);
	std::shared_ptr<Feimos::Texture<float>> bumpMap = std::make_shared<Feimos::ConstantTexture<float>>(0.0f);
	//材质
	std::shared_ptr<Feimos::Material> dragonMaterial = std::make_shared<Feimos::MatteMaterial>(KdDragon, sigma, bumpMap);
	std::shared_ptr<Feimos::Material> floorMaterial = std::make_shared<Feimos::MatteMaterial>(KdFloor, sigma, bumpMap);

	//地板
	emit PrintString("Init Floor");
	int nTrianglesFloor = 2;
	int vertexIndicesFloor[6] = {0, 1, 2, 3, 4, 5};
	int nVerticesFloor = 6;
	const float yPos_Floor = -2.0;
	Feimos::Point3f P_Floor[6] = {
		Feimos::Point3f(-6.0, yPos_Floor, 6.0), Feimos::Point3f(6.0, yPos_Floor, 6.0), Feimos::Point3f(-6.0, yPos_Floor, -6.0),
		Feimos::Point3f(6.0, yPos_Floor, 6.0), Feimos::Point3f(6.0, yPos_Floor, -6.0), Feimos::Point3f(-6.0, yPos_Floor, -6.0)};
	Feimos::Transform floor_Object2World;
	Feimos::Transform floor_World2Object = Feimos::Inverse(floor_Object2World);
	std::shared_ptr<Feimos::TriangleMesh> meshFloor = std::make_shared<Feimos::TriangleMesh>(floor_Object2World, nTrianglesFloor, vertexIndicesFloor, nVerticesFloor, P_Floor, nullptr, nullptr, nullptr, nullptr);
	std::vector<std::shared_ptr<Feimos::Shape>> trisFloor;
	for (int i = 0; i < nTrianglesFloor; ++i)
		trisFloor.push_back(std::make_shared<Feimos::Triangle>(&floor_Object2World, &floor_World2Object, false, meshFloor, i));

	// 生成Mesh加速结构
	std::shared_ptr<Feimos::TriangleMesh> mesh;
	std::vector<std::shared_ptr<Feimos::Shape>> tris;
	std::vector<std::shared_ptr<Feimos::Primitive>> prims;
	std::shared_ptr<Feimos::Aggregate> aggregate;
	Feimos::Transform tri_Object2World, tri_World2Object;

	tri_Object2World = Feimos::Translate(Feimos::Vector3f(0.0, -2.5, 0.0)) * tri_Object2World;
	tri_World2Object = Feimos::Inverse(tri_Object2World);

	emit PrintString("Read Mesh");
	Feimos::plyInfo plyi("../../Resources/dragon.3d");
	mesh = std::make_shared<Feimos::TriangleMesh>(tri_Object2World, plyi.nTriangles, plyi.vertexIndices, plyi.nVertices, plyi.vertexArray, nullptr, nullptr, nullptr, nullptr);
	tris.reserve(plyi.nTriangles);
	emit PrintString("Init Triangles");
	for (int i = 0; i < plyi.nTriangles; ++i)
		tris.push_back(std::make_shared<Feimos::Triangle>(&tri_Object2World, &tri_World2Object, false, mesh, i));

	emit PrintString("Init Primitives");
	for (int i = 0; i < plyi.nTriangles; ++i)
		prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(tris[i], dragonMaterial));
	for (int i = 0; i < nTrianglesFloor; ++i)
		prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisFloor[i], floorMaterial));

	emit PrintString("Init Accelerator");
	aggregate = std::make_unique<Feimos::BVHAccel>(prims, 1);
	plyi.Release();

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
		emit PrintString("test");
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
