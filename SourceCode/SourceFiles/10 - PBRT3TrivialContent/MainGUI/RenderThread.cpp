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
#include "Integrator/WhittedIntegrator.h"

#include "Material/Material.h"
#include "Material/MatteMaterial.h"
#include "Material/Mirror.h"

#include "Texture/Texture.h"
#include "Texture/ConstantTexture.h"

#include "Light/Light.h"
#include "Light/DiffuseLight.h"
#include "Light/PointLight.h"
#include "Light/SkyBoxLight.h"

#include "RenderStatus.h"

void showMemoryInfo(void);

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
	emit PrintString("Prepared to Render...");

	ClockRandomInit();

	int WIDTH = 500;
	int HEIGHT = 500;

	emit PrintString("Init FrameBuffer...");
	p_framebuffer->bufferResize(WIDTH, HEIGHT);

	emit PrintString("Init Camera...");
	std::shared_ptr<Feimos::Camera> camera;
	//初始化程序
	Feimos::Point3f eye(-4.0f, 1.f, -4.0f), look(0.0, 0.0, 0.0f);
	Feimos::Vector3f up(0.0f, 1.0f, 0.0f);
	Feimos::Transform lookat = LookAt(eye, look, up);
	//取逆是因为LookAt返回的是世界坐标到相机坐标系的变换
	//而我们需要相机坐标系到世界坐标系的变换
	Feimos::Transform Camera2World = Feimos::Inverse(lookat);
	camera = std::shared_ptr<Feimos::Camera>(Feimos::CreatePerspectiveCamera(WIDTH, HEIGHT, Camera2World));

	// 生成材质与纹理
	emit PrintString("Init Material...");
	Feimos::Spectrum floorColor;
	floorColor[0] = 0.2;
	floorColor[1] = 0.3;
	floorColor[2] = 0.9;
	Feimos::Spectrum dragonColor;
	dragonColor[0] = 1.0;
	dragonColor[1] = 1.0;
	dragonColor[2] = 0.0;
	std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> KdDragon = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(dragonColor);
	std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> KrDragon = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(dragonColor);
	std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> KdFloor = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(floorColor);
	std::shared_ptr<Feimos::Texture<float>> sigma = std::make_shared<Feimos::ConstantTexture<float>>(0.0f);
	std::shared_ptr<Feimos::Texture<float>> bumpMap = std::make_shared<Feimos::ConstantTexture<float>>(0.0f);
	//材质
	std::shared_ptr<Feimos::Material> dragonMaterial = std::make_shared<Feimos::MatteMaterial>(KdDragon, sigma, bumpMap);
	std::shared_ptr<Feimos::Material> floorMaterial = std::make_shared<Feimos::MatteMaterial>(KdFloor, sigma, bumpMap);
	std::shared_ptr<Feimos::Material> whiteLightMaterial = std::make_shared<Feimos::MatteMaterial>(KdFloor, sigma, bumpMap);
	std::shared_ptr<Feimos::Material> mirrorMaterial = std::make_shared<Feimos::MirrorMaterial>(KrDragon, bumpMap);

	//地板
	emit PrintString("Init Floor...");
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

	// 生成Mesh与加速结构
	std::shared_ptr<Feimos::TriangleMesh> mesh;
	std::vector<std::shared_ptr<Feimos::Shape>> tris;
	std::vector<std::shared_ptr<Feimos::Primitive>> prims;
	std::shared_ptr<Feimos::Aggregate> aggregate;
	Feimos::Transform tri_Object2World, tri_World2Object;

	tri_Object2World = Feimos::Translate(Feimos::Vector3f(0.0, -2.5, 0.0)) * tri_Object2World;
	tri_World2Object = Feimos::Inverse(tri_Object2World);

	emit PrintString("Read Mesh...");
	Feimos::plyInfo plyi("../../Resources/dragon.3d");
	mesh = std::make_shared<Feimos::TriangleMesh>(tri_Object2World, plyi.nTriangles, plyi.vertexIndices, plyi.nVertices, plyi.vertexArray, nullptr, nullptr, nullptr, nullptr);
	tris.reserve(plyi.nTriangles);
	emit PrintString("Init Triangles...");
	for (int i = 0; i < plyi.nTriangles; ++i)
		tris.push_back(std::make_shared<Feimos::Triangle>(&tri_Object2World, &tri_World2Object, false, mesh, i));

	emit PrintString("Init Primitives...");
	for (int i = 0; i < plyi.nTriangles; ++i)
		prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(tris[i], dragonMaterial, nullptr));
	for (int i = 0; i < nTrianglesFloor; ++i)
		prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisFloor[i], floorMaterial, nullptr));

	//面光源
	emit PrintString("Init AreaLight...");
	// 光源
	std::vector<std::shared_ptr<Feimos::Light>> lights;

	// 定义面光源
	int nTrianglesAreaLight = 2;						//面光源数（三角形数）
	int vertexIndicesAreaLight[6] = {0, 1, 2, 3, 4, 5}; //面光源顶点索引
	int nVerticesAreaLight = 6;							//面光源顶点数
	const float yPos_AreaLight = 0.0;
	Feimos::Point3f P_AreaLight[6] = {Feimos::Point3f(-1.4, 0.0, 1.4), Feimos::Point3f(-1.4, 0.0, -1.4), Feimos::Point3f(1.4, 0.0, 1.4),
									  Feimos::Point3f(1.4, 0.0, 1.4), Feimos::Point3f(-1.4, 0.0, -1.4), Feimos::Point3f(1.4, 0.0, -1.4)};
	//面光源的变换矩阵
	Feimos::Transform tri_Object2World_AreaLight = Feimos::Translate(Feimos::Vector3f(0.7f, 5.0f, -2.0f));
	Feimos::Transform tri_World2Object_AreaLight = Feimos::Inverse(tri_Object2World_AreaLight);
	//构造三角面片集
	std::shared_ptr<Feimos::TriangleMesh> meshAreaLight = std::make_shared<Feimos::TriangleMesh>(tri_Object2World_AreaLight, nTrianglesAreaLight, vertexIndicesAreaLight, nVerticesAreaLight, P_AreaLight, nullptr, nullptr, nullptr, nullptr);
	std::vector<std::shared_ptr<Feimos::Shape>> trisAreaLight;
	//生成三角形数组
	for (int i = 0; i < nTrianglesAreaLight; ++i)
		trisAreaLight.push_back(std::make_shared<Feimos::Triangle>(&tri_Object2World_AreaLight, &tri_World2Object_AreaLight, false, meshAreaLight, i));
	//填充光源类物体到基元
	for (int i = 0; i < nTrianglesAreaLight; ++i)
	{
		// std::shared_ptr<Feimos::AreaLight> area =
		//	std::make_shared<Feimos::DiffuseAreaLight>(tri_Object2World_AreaLight, Feimos::Spectrum(5.f), 5, trisAreaLight[i], false);
		// lights.push_back(area);
		// prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisAreaLight[i], whiteLightMaterial, area));
	}

	emit PrintString("Init Accelerator...");
	aggregate = std::make_unique<Feimos::BVHAccel>(prims, 1);
	plyi.Release();

	//无限环境光源
	emit PrintString("Init SkyBoxLight...");
	Feimos::Transform SkyBoxToWorld;
	Feimos::Point3f SkyBoxCenter(0.f, 0.f, 0.f);
	float SkyBoxRadius = 10.0f;
	std::shared_ptr<Feimos::Light> skyBoxLight =
		std::make_shared<Feimos::SkyBoxLight>(SkyBoxToWorld, SkyBoxCenter, SkyBoxRadius, "Resources/TropicalRuins1000.hdr", 1);
	lights.push_back(skyBoxLight);

	emit PrintString("Init Sampler...");
	Feimos::Bounds2i imageBound(Feimos::Point2i(0, 0), Feimos::Point2i(WIDTH, HEIGHT));
	std::shared_ptr<Feimos::ClockRandSampler> sampler = std::make_unique<Feimos::ClockRandSampler>(8, imageBound);

	emit PrintString("Build Scene...");
	std::unique_ptr<Feimos::Scene> worldScene = std::make_unique<Feimos::Scene>(aggregate, lights);
	Feimos::Bounds2i ScreenBound(Feimos::Point2i(0, 0), Feimos::Point2i(WIDTH, HEIGHT));

	emit PrintString("Build Integrator...");
	std::shared_ptr<Feimos::Integrator> integrator = std::make_shared<Feimos::WhittedIntegrator>(15, camera, sampler, ScreenBound, p_framebuffer);

	emit PrintString("Start Rendering!");
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

		showMemoryInfo();
	}

	emit PrintString("End Rendering.");
}

#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
void showMemoryInfo(void)
{

	//  SIZE_T PeakWorkingSetSize; //峰值内存使用
	//  SIZE_T WorkingSetSize; //内存使用
	//  SIZE_T PagefileUsage; //虚拟内存使用
	//  SIZE_T PeakPagefileUsage; //峰值虚拟内存使用

	EmptyWorkingSet(GetCurrentProcess());

	HANDLE handle = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));

	// DebugText::getDebugText()->addContents("Memory Use: WorkingSetSize: " + QString::number(pmc.WorkingSetSize / 1000.f / 1000.f) + " M");
	// DebugText::getDebugText()->addContents("PeakWorkingSetSize: " + QString::number(pmc.PeakWorkingSetSize / 1000.f / 1000.f) + " M");
	// DebugText::getDebugText()->addContents("PagefileUsage: " + QString::number(pmc.PagefileUsage / 1000.f / 1000.f) + " M");
	// DebugText::getDebugText()->addContents("PeakPagefileUsage: " + QString::number(pmc.PeakPagefileUsage / 1000.f / 1000.f) + " M");

	m_RenderStatus.setDataChanged("Memory Use", "WorkingSetSize", QString::number(pmc.WorkingSetSize / 1000.f / 1000.f), "M");
	m_RenderStatus.setDataChanged("Memory Use", "PeakWorkingSetSize", QString::number(pmc.PeakWorkingSetSize / 1000.f / 1000.f), "M");
	m_RenderStatus.setDataChanged("Memory Use", "PagefileUsage", QString::number(pmc.PagefileUsage / 1000.f / 1000.f), "M");
	m_RenderStatus.setDataChanged("Memory Use", "PeakPagefileUsage", QString::number(pmc.PeakPagefileUsage / 1000.f / 1000.f), "M");
}
