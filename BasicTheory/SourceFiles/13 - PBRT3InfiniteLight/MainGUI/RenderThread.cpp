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
#include "Integrator/DirectLightingIntegrator.h"
#include "Integrator/PathIntegrator.h"

#include "Material/Material.h"
#include "Material/MatteMaterial.h"
#include "Material/Mirror.h"
#include "Material/MetalMaterial.h"
#include "Material/GlassMaterial.h"

#include "Texture/Texture.h"
#include "Texture/ConstantTexture.h"

#include "Light/Light.h"
#include "Light/DiffuseLight.h"
#include "Light/PointLight.h"
#include "Light/SkyBoxLight.h"
#include "Light/InfiniteAreaLight.h"

#define windows_operating_system false
#if windows_operating_system
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
void showMemoryInfo(void)
{

	//  SIZE_T PeakWorkingSetSize; //?????????
	//  SIZE_T WorkingSetSize; //??????
	//  SIZE_T PagefileUsage; //??????????
	//  SIZE_T PeakPagefileUsage; //?????????????

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

inline std::shared_ptr<Feimos::Material> getGreeyMatteMaterial()
{
	Feimos::Spectrum whiteColor;
	whiteColor[0] = 0.41;
	whiteColor[1] = 0.41;
	whiteColor[2] = 0.41;
	std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> KdWhite = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(whiteColor);
	std::shared_ptr<Feimos::Texture<float>> sigma = std::make_shared<Feimos::ConstantTexture<float>>(0.0f);
	std::shared_ptr<Feimos::Texture<float>> bumpMap = std::make_shared<Feimos::ConstantTexture<float>>(0.0f);
	// 材质
	return std::make_shared<Feimos::MatteMaterial>(KdWhite, sigma, bumpMap);
}

inline std::shared_ptr<Feimos::Material> getYelloMetalMaterial()
{
	Feimos::Spectrum eta;
	eta[0] = 0.2f;
	eta[1] = 0.2f;
	eta[2] = 0.8f;
	std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> etaM = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(eta);
	Feimos::Spectrum k;
	k[0] = 0.11f;
	k[1] = 0.11f;
	k[2] = 0.11f;
	std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> kM = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(k);
	std::shared_ptr<Feimos::Texture<float>> Roughness = std::make_shared<Feimos::ConstantTexture<float>>(0.15f);
	std::shared_ptr<Feimos::Texture<float>> RoughnessU = std::make_shared<Feimos::ConstantTexture<float>>(0.15f);
	std::shared_ptr<Feimos::Texture<float>> RoughnessV = std::make_shared<Feimos::ConstantTexture<float>>(0.15f);
	std::shared_ptr<Feimos::Texture<float>> bumpMap = std::make_shared<Feimos::ConstantTexture<float>>(0.0f);
	return std::make_shared<Feimos::MetalMaterial>(etaM, kM, Roughness, RoughnessU, RoughnessV, bumpMap, false);
}

inline std::shared_ptr<Feimos::Material> getWhiteGlassMaterial()
{
	Feimos::Spectrum c1;
	c1[0] = 0.98f;
	c1[1] = 0.98f;
	c1[2] = 0.98f;
	std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> Kr = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(c1);
	Feimos::Spectrum c2;
	c2[0] = 0.98f;
	c2[1] = 0.98f;
	c2[2] = 0.98f;
	std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> Kt = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(c2);
	std::shared_ptr<Feimos::Texture<float>> index = std::make_shared<Feimos::ConstantTexture<float>>(1.5f);
	std::shared_ptr<Feimos::Texture<float>> RoughnessU = std::make_shared<Feimos::ConstantTexture<float>>(0.1f);
	std::shared_ptr<Feimos::Texture<float>> RoughnessV = std::make_shared<Feimos::ConstantTexture<float>>(0.1f);
	std::shared_ptr<Feimos::Texture<float>> bumpMap = std::make_shared<Feimos::ConstantTexture<float>>(0.0f);
	return std::make_shared<Feimos::GlassMaterial>(Kr, Kt,
												   RoughnessU, RoughnessV, index, bumpMap, false);
}

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

	int WIDTH = 720;
	int HEIGHT = 500;

	emit PrintString("Init FrameBuffer...");
	p_framebuffer->bufferResize(WIDTH, HEIGHT);

	emit PrintString("Init Camera...");
	std::shared_ptr<Feimos::Camera> camera;
	//初始化程序
	Feimos::Point3f eye(0.f, 2.f, 6.f), look(0.f, 0.f, 0.0f);
	Feimos::Vector3f up(0.0f, 1.0f, 0.0f);
	Feimos::Transform lookat = LookAt(eye, look, up);
	//取逆是因为LookAt返回的是世界坐标到相机坐标系的变换
	//而我们需要相机坐标系到世界坐标系的变换
	Feimos::Transform Camera2World = Inverse(lookat);
	camera = std::shared_ptr<Feimos::Camera>(Feimos::CreatePerspectiveCamera(WIDTH, HEIGHT, Camera2World));

	// 生成材质与纹理
	emit PrintString("Init Material...");
	std::shared_ptr<Feimos::Material> dragonMaterial;
	std::shared_ptr<Feimos::Material> whiteWallMaterial;
	std::shared_ptr<Feimos::Material> redWallMaterial;
	std::shared_ptr<Feimos::Material> blueWallMaterial;
	std::shared_ptr<Feimos::Material> whiteLightMaterial;
	std::shared_ptr<Feimos::Material> mirrorMaterial;
	std::shared_ptr<Feimos::Material> yellowMetalMaterial = getYelloMetalMaterial();
	std::shared_ptr<Feimos::Material> whiteGlassMaterial = getWhiteGlassMaterial();
	std::shared_ptr<Feimos::Material> greeyDiffuseMaterial = getGreeyMatteMaterial();
	{
		Feimos::Spectrum whiteColor;
		whiteColor[0] = 0.91;
		whiteColor[1] = 0.91;
		whiteColor[2] = 0.91;
		Feimos::Spectrum dragonColor;
		dragonColor[0] = 1.0;
		dragonColor[1] = 1.0;
		dragonColor[2] = 0.0;
		Feimos::Spectrum redWallColor;
		redWallColor[0] = 0.9;
		redWallColor[1] = 0.1;
		redWallColor[2] = 0.17;
		Feimos::Spectrum blueWallColor;
		blueWallColor[0] = 0.14;
		blueWallColor[1] = 0.21;
		blueWallColor[2] = 0.87;
		std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> KdDragon = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(dragonColor);
		std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> KrDragon = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(dragonColor);
		std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> KdWhite = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(whiteColor);
		std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> KdRed = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(redWallColor);
		std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> KdBlue = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(blueWallColor);
		std::shared_ptr<Feimos::Texture<float>> sigma = std::make_shared<Feimos::ConstantTexture<float>>(0.0f);
		std::shared_ptr<Feimos::Texture<float>> bumpMap = std::make_shared<Feimos::ConstantTexture<float>>(0.0f);
		// 材质
		dragonMaterial = std::make_shared<Feimos::MatteMaterial>(KdDragon, sigma, bumpMap);

		whiteWallMaterial = std::make_shared<Feimos::MatteMaterial>(KdWhite, sigma, bumpMap);
		redWallMaterial = std::make_shared<Feimos::MatteMaterial>(KdRed, sigma, bumpMap);
		blueWallMaterial = std::make_shared<Feimos::MatteMaterial>(KdBlue, sigma, bumpMap);

		whiteLightMaterial = std::make_shared<Feimos::MatteMaterial>(KdWhite, sigma, bumpMap);
		mirrorMaterial = std::make_shared<Feimos::MirrorMaterial>(KrDragon, bumpMap);
	}

	std::vector<std::shared_ptr<Feimos::Primitive>> prims;

	// emit PrintString("Init Cornell Box...");
	// {
	// 	//墙和地板
	// 	const int nTrianglesWall = 2 * 5;
	// 	int vertexIndicesWall[nTrianglesWall * 3];
	// 	for (int i = 0; i < nTrianglesWall * 3; i++)
	// 		vertexIndicesWall[i] = i;
	// 	const int nVerticesWall = nTrianglesWall * 3;
	// 	const float length_Wall = 5.0f;
	// 	Feimos::Point3f P_Wall[nVerticesWall] = {
	// 		//底座
	// 		Feimos::Point3f(0.f, 0.f, length_Wall), Feimos::Point3f(length_Wall, 0.f, length_Wall), Feimos::Point3f(0.f, 0.f, 0.f),
	// 		Feimos::Point3f(length_Wall, 0.f, length_Wall), Feimos::Point3f(length_Wall, 0.f, 0.f), Feimos::Point3f(0.f, 0.f, 0.f),
	// 		//天花板
	// 		Feimos::Point3f(0.f, length_Wall, length_Wall), Feimos::Point3f(0.f, length_Wall, 0.f), Feimos::Point3f(length_Wall, length_Wall, length_Wall),
	// 		Feimos::Point3f(length_Wall, length_Wall, length_Wall), Feimos::Point3f(0.f, length_Wall, 0.f), Feimos::Point3f(length_Wall, length_Wall, 0.f),
	// 		//后墙
	// 		Feimos::Point3f(0.f, 0.f, 0.f), Feimos::Point3f(length_Wall, 0.f, 0.f), Feimos::Point3f(length_Wall, length_Wall, 0.f),
	// 		Feimos::Point3f(0.f, 0.f, 0.f), Feimos::Point3f(length_Wall, length_Wall, 0.f), Feimos::Point3f(0.f, length_Wall, 0.f),
	// 		//右墙
	// 		Feimos::Point3f(0.f, 0.f, 0.f), Feimos::Point3f(0.f, length_Wall, length_Wall), Feimos::Point3f(0.f, 0.f, length_Wall),
	// 		Feimos::Point3f(0.f, 0.f, 0.f), Feimos::Point3f(0.f, length_Wall, 0.f), Feimos::Point3f(0.f, length_Wall, length_Wall),
	// 		//左墙
	// 		Feimos::Point3f(length_Wall, 0.f, 0.f), Feimos::Point3f(length_Wall, length_Wall, length_Wall), Feimos::Point3f(length_Wall, 0.f, length_Wall),
	// 		Feimos::Point3f(length_Wall, 0.f, 0.f), Feimos::Point3f(length_Wall, length_Wall, 0.f), Feimos::Point3f(length_Wall, length_Wall, length_Wall)};
	// 	Feimos::Transform tri_ConBox2World = Feimos::Translate(Feimos::Vector3f(-0.5 * length_Wall, -0.5 * length_Wall, -0.5 * length_Wall));
	// 	Feimos::Transform tri_World2ConBox = Feimos::Inverse(tri_ConBox2World);
	// 	std::shared_ptr<Feimos::TriangleMesh> meshConBox = std::make_shared<Feimos::TriangleMesh>(tri_ConBox2World, nTrianglesWall, vertexIndicesWall, nVerticesWall, P_Wall, nullptr, nullptr, nullptr, nullptr);
	// 	std::vector<std::shared_ptr<Feimos::Shape>> trisConBox;
	// 	for (int i = 0; i < nTrianglesWall; ++i)
	// 		trisConBox.push_back(std::make_shared<Feimos::Triangle>(&tri_ConBox2World, &tri_World2ConBox, false, meshConBox, i));
	// 	for (int i = 0; i < nTrianglesWall; ++i)
	// 	{
	// 		if (i == 6 || i == 7)
	// 			prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisConBox[i], redWallMaterial, nullptr));
	// 		else if (i == 8 || i == 9)
	// 			prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisConBox[i], blueWallMaterial, nullptr));
	// 		else
	// 			prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisConBox[i], whiteWallMaterial, nullptr));
	// 	}
	// }

	emit PrintString("Init Floor...");
	{
		//墙和地板
		const int nTrianglesWall = 2;
		int vertexIndicesWall[nTrianglesWall * 3];
		for (int i = 0; i < nTrianglesWall * 3; i++)
			vertexIndicesWall[i] = i;
		const int nVerticesWall = nTrianglesWall * 3;
		const float length_Wall = 8.0f;
		Feimos::Point3f P_Wall[nVerticesWall] = {
			//底座
			Feimos::Point3f(0.f, 0.f, length_Wall),
			Feimos::Point3f(length_Wall, 0.f, length_Wall),
			Feimos::Point3f(0.f, 0.f, 0.f),
			Feimos::Point3f(length_Wall, 0.f, length_Wall),
			Feimos::Point3f(length_Wall, 0.f, 0.f),
			Feimos::Point3f(0.f, 0.f, 0.f),
		};
		Feimos::Transform tri_Floor2World = Feimos::Translate(Feimos::Vector3f(-0.5 * length_Wall, -0.5 * length_Wall, -0.5 * length_Wall));
		Feimos::Transform tri_World2Floor = Feimos::Inverse(tri_Floor2World);
		std::shared_ptr<Feimos::TriangleMesh> meshConBox = std::make_shared<Feimos::TriangleMesh>(tri_Floor2World, nTrianglesWall, vertexIndicesWall, nVerticesWall, P_Wall, nullptr, nullptr, nullptr, nullptr);
		std::vector<std::shared_ptr<Feimos::Shape>> trisFloor;
		for (int i = 0; i < nTrianglesWall; ++i)
			trisFloor.push_back(std::make_shared<Feimos::Triangle>(&tri_Floor2World, &tri_World2Floor, false, meshConBox, i));

		//将物体填充到基元
		for (int i = 0; i < nTrianglesWall; ++i)
		{
			prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisFloor[i], greeyDiffuseMaterial, nullptr));
		}
	}

	emit PrintString("Init Mesh...");
	{
		std::shared_ptr<Feimos::TriangleMesh> mesh;
		std::vector<std::shared_ptr<Feimos::Shape>> tris;

		Feimos::Transform tri_Object2World, tri_World2Object;

		tri_Object2World = Feimos::Translate(Feimos::Vector3f(0.f, -4.9f, 0.f)) * tri_Object2World;
		tri_World2Object = Inverse(tri_Object2World);

		emit PrintString("   Read Mesh...");
		Feimos::plyInfo plyi("../../Resources/dragon.3d");
		mesh = std::make_shared<Feimos::TriangleMesh>(tri_Object2World, plyi.nTriangles, plyi.vertexIndices, plyi.nVertices, plyi.vertexArray, nullptr, nullptr, nullptr, nullptr);
		tris.reserve(plyi.nTriangles);
		emit PrintString("   Init Triangles...");
		for (int i = 0; i < plyi.nTriangles; ++i)
			tris.push_back(std::make_shared<Feimos::Triangle>(&tri_Object2World, &tri_World2Object, false, mesh, i));

		emit PrintString("   Init Primitives...");
		for (int i = 0; i < plyi.nTriangles; ++i)
			prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(tris[i], dragonMaterial, nullptr));
		plyi.Release();
	}

	// 光源

	//面光源
	emit PrintString("Init AreaLight...");
	std::vector<std::shared_ptr<Feimos::Light>> lights;
	{
		// 定义面光源
		int nTrianglesAreaLight = 2;						//面光源数（三角形数）
		int vertexIndicesAreaLight[6] = {0, 1, 2, 3, 4, 5}; //面光源顶点索引
		int nVerticesAreaLight = 6;							//面光源顶点数
		const float yPos_AreaLight = 0.0;
		Feimos::Point3f P_AreaLight[6] = {Feimos::Point3f(-1.4, 0.0, 1.4), Feimos::Point3f(-1.4, 0.0, -1.4), Feimos::Point3f(1.4, 0.0, 1.4),
										  Feimos::Point3f(1.4, 0.0, 1.4), Feimos::Point3f(-1.4, 0.0, -1.4), Feimos::Point3f(1.4, 0.0, -1.4)};
		//面光源的变换矩阵
		Feimos::Transform tri_Object2World_AreaLight = Feimos::Translate(Feimos::Vector3f(0.0f, 2.45f, 0.0f));
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
			// lights.push_back(area);
			// prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisAreaLight[i], whiteLightMaterial, area));
		}
	}

	//天空盒
	emit PrintString("Init SkyBoxLight...");
	{
		Feimos::Transform SkyBoxToWorld;
		Feimos::Point3f SkyBoxCenter(0.f, 0.f, 0.f);
		float SkyBoxRadius = 10.0f;
		// std::shared_ptr<Feimos::Light> skyBoxLight =
		//	std::make_shared<Feimos::SkyBoxLight>(SkyBoxToWorld, SkyBoxCenter, SkyBoxRadius, "../../Resources/TropicalRuins1000.hdr", 1);
		// lights.push_back(skyBoxLight);
	}
	//无限环境光源
	emit PrintString("Init InfiniteLight...");
	{
		Feimos::Transform InfinityLightToWorld = Feimos::RotateX(20) * Feimos::RotateY(-90) * Feimos::RotateX(-90);
		Feimos::Spectrum power(1.0f);
		std::shared_ptr<Feimos::Light> infinityLight =
			std::make_shared<Feimos::InfiniteAreaLight>(InfinityLightToWorld, power, 10, "../../Resources/MonValley1000.hdr");
		lights.push_back(infinityLight);
	}

	// 生成加速结构
	emit PrintString("Init Accelerator...");
	std::shared_ptr<Feimos::Aggregate> aggregate;
	aggregate = std::make_unique<Feimos::BVHAccel>(prims, 1);

	emit PrintString("Init Sampler...");
	Feimos::Bounds2i imageBound(Feimos::Point2i(0, 0), Feimos::Point2i(WIDTH, HEIGHT));
	std::shared_ptr<Feimos::ClockRandSampler> sampler = std::make_unique<Feimos::ClockRandSampler>(8, imageBound);

	emit PrintString("Build Scene...");
	std::unique_ptr<Feimos::Scene> worldScene = std::make_unique<Feimos::Scene>(aggregate, lights);
	Feimos::Bounds2i ScreenBound(Feimos::Point2i(0, 0), Feimos::Point2i(WIDTH, HEIGHT));

	emit PrintString("Build Integrator...");
	std::shared_ptr<Feimos::Integrator> integrator = std::make_shared<Feimos::PathIntegrator>(15, camera, sampler, ScreenBound, 1.f, "spatial", p_framebuffer);
	// std::shared_ptr<Feimos::Integrator> integrator = std::make_shared<Feimos::WhittedIntegrator>(15, camera, sampler, ScreenBound, p_framebuffer);

	emit PrintString("Start Rendering!");
	// 开始执行渲染
	int renderCount = 0;
	while (renderFlag)
	{
		QElapsedTimer t;
		t.start();

		double frameTime;
		integrator->Render(*worldScene, frameTime);

#if windows_operating_system
		m_RenderStatus.setDataChanged("Performance", "One Frame Time", QString::number(frameTime), "");
		m_RenderStatus.setDataChanged("Performance", "Frame pre second", QString::number(1.0f / (float)frameTime), "");
#endif

		emit PaintBuffer(p_framebuffer->getUCbuffer(), WIDTH, HEIGHT, 4);

		while (t.elapsed() < 1)
			;

#if windows_operating_system
		showMemoryInfo();
#endif
	}

	emit PrintString("End Rendering.");
}