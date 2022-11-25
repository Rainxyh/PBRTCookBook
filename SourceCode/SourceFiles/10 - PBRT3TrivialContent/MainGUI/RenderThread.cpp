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
#include "Material/PerfectGlassMaterial.h"

#include "Texture/Texture.h"
#include "Texture/ConstantTexture.h"

#include "Light/Light.h"
#include "Light/DiffuseLight.h"
#include "Light/PointLight.h"
#include "Light/SkyBoxLight.h"

#include "RenderStatus.h"

#define windows_operating_system false
#if windows_operating_system
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
#endif

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
	Feimos::Point3f eye(2.5, 2.5, 6.0), look(2.5, 2.5, 0.0);
	Feimos::Vector3f up(0.0f, 1.0f, 0.0f);
	Feimos::Transform lookat = LookAt(eye, look, up);
	//取逆是因为LookAt返回的是世界坐标到相机坐标系的变换
	//而我们需要相机坐标系到世界坐标系的变换
	Feimos::Transform Camera2World = Feimos::Inverse(lookat);
	camera = std::shared_ptr<Feimos::Camera>(Feimos::CreatePerspectiveCamera(WIDTH, HEIGHT, Camera2World));

	// 生成材质与纹理
	emit PrintString("Init Material...");
	Feimos::Spectrum floorColor(0.f, 0.f, 250.f / 255.f);
	Feimos::Spectrum dragonColor(.2f, .7f, .2f);
	Feimos::Spectrum lightColor(1.f);
	Feimos::Spectrum mirrorColor(1.f);
	Feimos::Spectrum red(1.f, 0.f, 0.f);
	Feimos::Spectrum green(0.f, 1.f, 0.f);
	Feimos::Spectrum blue(0.f, 0.f, 1.f);
	Feimos::Spectrum gray(0.7f, 0.7f, 0.7f);

	std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> KdDragon = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(dragonColor);
	std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> KdFloor = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(floorColor);
	std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> KdLight = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(lightColor);
	std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> KdMirror = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(mirrorColor);
	std::shared_ptr<Feimos::Texture<float>> sigma = std::make_shared<Feimos::ConstantTexture<float>>(0.0f);
	std::shared_ptr<Feimos::Texture<float>> bumpMap = std::make_shared<Feimos::ConstantTexture<float>>(0.0f);
	std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> glassKr = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(1.f);
	std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> glassKt = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(1.f);
	std::shared_ptr<Feimos::Texture<float>> glassEta = std::make_shared<Feimos::ConstantTexture<float>>(1.2f);

	//材质
	std::shared_ptr<Feimos::Material> dragonMaterial = std::make_shared<Feimos::MatteMaterial>(KdDragon, sigma, bumpMap);
	std::shared_ptr<Feimos::Material> floorMaterial = std::make_shared<Feimos::MatteMaterial>(KdFloor, sigma, bumpMap);
	std::shared_ptr<Feimos::Material> whiteLightMaterial = std::make_shared<Feimos::MatteMaterial>(KdLight, sigma, bumpMap);
	std::shared_ptr<Feimos::Material> mirrorMaterial = std::make_shared<Feimos::MirrorMaterial>(KdMirror, bumpMap);
	std::shared_ptr<Feimos::Material> glassMaterial = std::make_shared<Feimos::PerfectGlassMaterial>(glassKr, glassKt, glassEta, bumpMap);

	//地板
	emit PrintString("Init Floor...");
	int nTrianglesFloor = 2;
	const int nVerticesFloor = 3 * nTrianglesFloor;
	int vertexIndicesFloor[] = {0, 1, 2, 3, 4, 5};
	const float yPos_Floor = -2.0;
	Feimos::Point3f P_Floor[] = {
		Feimos::Point3f(-6.0, yPos_Floor, 6.0), Feimos::Point3f(6.0, yPos_Floor, 6.0), Feimos::Point3f(-6.0, yPos_Floor, -6.0),
		Feimos::Point3f(6.0, yPos_Floor, 6.0), Feimos::Point3f(6.0, yPos_Floor, -6.0), Feimos::Point3f(-6.0, yPos_Floor, -6.0)};

	Feimos::Transform floor_Object2World;
	Feimos::Transform floor_World2Object = Feimos::Inverse(floor_Object2World);
	std::shared_ptr<Feimos::TriangleMesh> meshFloor = std::make_shared<Feimos::TriangleMesh>(floor_Object2World, nTrianglesFloor, vertexIndicesFloor, nVerticesFloor, P_Floor, nullptr, nullptr, nullptr, nullptr);
	std::vector<std::shared_ptr<Feimos::Shape>> trisFloor;
	for (int i = 0; i < nTrianglesFloor; ++i)
		trisFloor.push_back(std::make_shared<Feimos::Triangle>(&floor_Object2World, &floor_World2Object, false, meshFloor, i));

	std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> redTex = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(red);
	std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> greenTex = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(green);
	std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> blueTex = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(blue);
	std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> grayTex = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(gray);
	std::shared_ptr<Feimos::Material> redMatteMaterial = std::make_shared<Feimos::MatteMaterial>(redTex, sigma, bumpMap);
	std::shared_ptr<Feimos::Material> greenMatteMaterial = std::make_shared<Feimos::MatteMaterial>(greenTex, sigma, bumpMap);
	std::shared_ptr<Feimos::Material> blueMatteMaterial = std::make_shared<Feimos::MatteMaterial>(blueTex, sigma, bumpMap);
	std::shared_ptr<Feimos::Material> grayMatteMaterial = std::make_shared<Feimos::MatteMaterial>(grayTex, sigma, bumpMap);
	int nTrianglesCornell = 10;
	int nVerticesCornell = 3 * nTrianglesCornell;
	int *vertexIndicesCornell = static_cast<int *>(malloc(sizeof(int) * nVerticesCornell));
	for (size_t i = 0; i < nVerticesCornell; ++i)
		vertexIndicesCornell[i] = i;
	// 康奈尔盒子
	const float length_Cornell = 5.f;
	Feimos::Point3f P_Cornell[] = {
		// 底座
		Feimos::Point3f(0.f, 0.f, length_Cornell),
		Feimos::Point3f(length_Cornell, 0.f, length_Cornell),
		Feimos::Point3f(0.f, 0.f, 0.f),
		Feimos::Point3f(length_Cornell, 0.f, length_Cornell),
		Feimos::Point3f(length_Cornell, 0.f, 0.f),
		Feimos::Point3f(0.f, 0.f, 0.f),
		// 天花板
		Feimos::Point3f(0.f, length_Cornell, length_Cornell),
		Feimos::Point3f(0.f, length_Cornell, 0.f),
		Feimos::Point3f(length_Cornell, length_Cornell, length_Cornell),
		Feimos::Point3f(length_Cornell, length_Cornell, length_Cornell),
		Feimos::Point3f(0.f, length_Cornell, 0.f),
		Feimos::Point3f(length_Cornell, length_Cornell, 0.f),
		// 后墙
		Feimos::Point3f(0.f, 0.f, 0.f),
		Feimos::Point3f(length_Cornell, 0.f, 0.f),
		Feimos::Point3f(length_Cornell, length_Cornell, 0.f),
		Feimos::Point3f(0.f, 0.f, 0.f),
		Feimos::Point3f(length_Cornell, length_Cornell, 0.f),
		Feimos::Point3f(0.f, length_Cornell, 0.f),
		// 右墙
		Feimos::Point3f(0.f, 0.f, 0.f),
		Feimos::Point3f(0.f, length_Cornell, length_Cornell),
		Feimos::Point3f(0.f, 0.f, length_Cornell),
		Feimos::Point3f(0.f, 0.f, 0.f),
		Feimos::Point3f(0.f, length_Cornell, 0.f),
		Feimos::Point3f(0.f, length_Cornell, length_Cornell),
		// 左墙
		Feimos::Point3f(length_Cornell, 0.f, 0.f),
		Feimos::Point3f(length_Cornell, length_Cornell, length_Cornell),
		Feimos::Point3f(length_Cornell, 0.f, length_Cornell),
		Feimos::Point3f(length_Cornell, 0.f, 0.f),
		Feimos::Point3f(length_Cornell, length_Cornell, 0.f),
		Feimos::Point3f(length_Cornell, length_Cornell, length_Cornell)};

	Feimos::Transform cornell_Object2World;
	Feimos::Transform cornell_World2Object = Feimos::Inverse(cornell_Object2World);
	std::shared_ptr<Feimos::TriangleMesh> meshCornell = std::make_shared<Feimos::TriangleMesh>(cornell_Object2World, nTrianglesCornell, vertexIndicesCornell, nVerticesCornell, P_Cornell, nullptr, nullptr, nullptr, nullptr);
	std::vector<std::shared_ptr<Feimos::Shape>> trisCornell;
	for (int i = 0; i < nTrianglesCornell; ++i)
		trisCornell.push_back(std::make_shared<Feimos::Triangle>(&cornell_Object2World, &cornell_World2Object, false, meshCornell, i));

	// 生成Mesh与加速结构
	std::shared_ptr<Feimos::TriangleMesh> mesh;
	std::vector<std::shared_ptr<Feimos::Shape>> tris;
	std::vector<std::shared_ptr<Feimos::Primitive>> prims;
	std::shared_ptr<Feimos::Aggregate> aggregate;
	Feimos::Transform tri_Object2World, tri_World2Object;

	// tri_Object2World = Feimos::Translate(Feimos::Vector3f(0.0, -2.5, 0.0)) * tri_Object2World;
	tri_Object2World = Feimos::Translate(Feimos::Vector3f(2.5, -1.0, 2.5)) * tri_Object2World;
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
	// for (int i = 0; i < nTrianglesFloor; ++i)
	// 	prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisFloor[i], floorMaterial, nullptr));
	for (int i = 0; i < nTrianglesCornell; ++i)
	{
		switch ((i / 2))
		{
		case 0: // 底座
			prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisCornell[i], grayMatteMaterial, nullptr));
			break;
		case 1: // 天花板
			prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisCornell[i], grayMatteMaterial, nullptr));
			break;
		case 2: // 后墙
			prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisCornell[i], grayMatteMaterial, nullptr));
			break;
		case 3: // 右墙
			prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisCornell[i], redMatteMaterial, nullptr));
			break;
		case 4: // 左墙
			prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisCornell[i], blueMatteMaterial, nullptr));
			break;
		default:
			break;
		}
	}

	//面光源
	emit PrintString("Init AreaLight...");
	// 光源
	std::vector<std::shared_ptr<Feimos::Light>> lights;

	// 定义面光源
	int nTrianglesAreaLight = 2;						//面光源数（三角形数）
	int vertexIndicesAreaLight[6] = {0, 1, 2, 3, 4, 5}; //面光源顶点索引
	int nVerticesAreaLight = 6;							//面光源顶点数
	const float yPos_AreaLight = 5.0f;
	Feimos::Point3f P_AreaLight[6] = {Feimos::Point3f(-1.f, yPos_AreaLight, 1.f), Feimos::Point3f(-1.f, yPos_AreaLight, -1.f), Feimos::Point3f(1.f, yPos_AreaLight, 1.f),
									  Feimos::Point3f(1.f, yPos_AreaLight, 1.f), Feimos::Point3f(-1.f, yPos_AreaLight, -1.f), Feimos::Point3f(1.f, yPos_AreaLight, -1.f)};
	//面光源的变换矩阵
	Feimos::Transform tri_Object2World_AreaLight = Feimos::Translate(Feimos::Vector3f(2.5f, -0.01f, 2.f));
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
		std::shared_ptr<Feimos::AreaLight> area =
			std::make_shared<Feimos::DiffuseAreaLight>(tri_Object2World_AreaLight, Feimos::Spectrum(5.f), 5, trisAreaLight[i], false);
		lights.push_back(area);
		prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisAreaLight[i], whiteLightMaterial, area));
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
		std::make_shared<Feimos::SkyBoxLight>(SkyBoxToWorld, SkyBoxCenter, SkyBoxRadius, "../../Resources/TropicalRuins1000.hdr", 1);
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

#if windows_operating_system
		showMemoryInfo();
#endif
	}

	emit PrintString("End Rendering.");
}