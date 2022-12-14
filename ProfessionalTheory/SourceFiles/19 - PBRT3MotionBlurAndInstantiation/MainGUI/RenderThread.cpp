#include "RenderThread.h"
#include "DebugText.hpp"
#include <QElapsedTimer>

#include "Core/FeimosRender.h"
#include "Core/Primitive.h"
#include "Core/Spectrum.h"
#include "Core/interaction.h"
#include "Core/Scene.h"
#include "Core/Transform.h"

#include "Shape/Triangle.h"
#include "Shape/plyRead.h"
#include "Shape/ModelLoad.h"
#include "Shape/RattlerLoad.h"

#include "Accelerator/BVHAccel.h"

#include "Camera/Camera.h"
#include "Camera/Perspective.h"

#include "Sampler/Sampler.h"
#include "Sampler/ClockRand.h"

#include "Integrator/Integrator.h"
#include "Integrator/WhittedIntegrator.h"
#include "Integrator/DirectLightingIntegrator.h"
#include "Integrator/PathIntegrator.h"
#include "Integrator/VolPathIntegrator.h"

#include "Material/Material.h"
#include "Material/MatteMaterial.h"
#include "Material/Mirror.h"
#include "Material/MetalMaterial.h"
#include "Material/GlassMaterial.h"
#include "Material/PlasticMaterial.h"

#include "Texture/Texture.h"
#include "Texture/ConstantTexture.h"
#include "Texture/ImageTexture.h"

#include "Light/Light.h"
#include "Light/DiffuseLight.h"
#include "Light/PointLight.h"
#include "Light/SkyBoxLight.h"
#include "Light/InfiniteAreaLight.h"

#include "Media/Medium.h"
#include "Media/HomogeneousMedium.h"
#include "Media/GridDensityMedium.h"

#include "Shape/ModelSet.h"
#include "Light/LightSet.h"
#include "Material/MaterialSet.h"

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

	int WIDTH = 1500;
	int HEIGHT = 1200;

	emit PrintString("Init FrameBuffer...");
	p_framebuffer->bufferResize(WIDTH, HEIGHT);

	emit PrintString("Init Camera...");
	std::shared_ptr<Feimos::Camera> camera;
	Feimos::Transform Cam2WorldStart, Cam2WorldEnd;
	// 初始化程序
	Feimos::Point3f eye(0.f, 0.f, 5.7f), look(0.f, 0.f, 0.0f);
	Feimos::Vector3f up(0.0f, 1.0f, 0.0f);
	Feimos::Transform lookat = LookAt(eye, look, up);
	// 取逆是因为LookAt返回的是世界坐标到相机坐标系的变换
	// 而我们需要相机坐标系到世界坐标系的变换
	Cam2WorldStart = Inverse(lookat);
	Cam2WorldEnd = Cam2WorldStart;
	Feimos::AnimatedTransform Camera2World(&Cam2WorldStart, 0.0f, &Cam2WorldEnd, 1.0f);
	camera = std::shared_ptr<Feimos::Camera>(Feimos::CreatePerspectiveCamera(WIDTH, HEIGHT, Camera2World, 0.0, 1.0));

	// 生成参与介质
	emit PrintString("Init Medium...");
	Feimos::MediumInterface noMedium;
	Feimos::HomogeneousMedium homoMedium(0.5, 4.4, -0.5);
	Feimos::MediumInterface homoMediumInterface(&homoMedium, nullptr);

	// 定义基元数组
	std::vector<std::shared_ptr<Feimos::Primitive>> prims;

	// 填充各种形状基元
	{
		emit PrintString("Init Cornell Box...");
		Feimos::Transform CorBox2World = Feimos::Translate(Feimos::Vector3f(-2.f, -2.0f, -2.0f)) * Feimos::Scale(4.0f, 4.0f, 4.0f);
		Feimos::getDiffuseCornellBox(CorBox2World, prims, noMedium);
	}

	{
		// emit PrintString("Init Floor...");
		// Feimos::Transform floor2World = Feimos::Translate(Feimos::Vector3f(-4.0f, -3.0f, -4.0f));
		// std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> tex = Feimos::getImageTexture("../../Resources/awesomeface.jpg");
		// std::shared_ptr<Feimos::Material> mat = Feimos::getMatteMaterial(tex);
		// Feimos::getDiffuseFloor(floor2World, 8.0f, mat, prims, noMedium);
	}

	{
		// emit PrintString("Init Smoke...");
		// float pMin[3] = {0.01, 0.01, 0.01}, pMax[3] = {1.99, 1.99, 0.79};
		// Feimos::Transform smoke2World = Feimos::Translate(Feimos::Vector3f(1.8f, -1.30f, 0.9f)) * Feimos::RotateY(180) * Feimos::Scale(2.0f, 2.0f, 2.0f);
		// Feimos::MediumLoad smokeMed("../../Resources/smokeVolume-1.volume", smoke2World);
		// Feimos::MediumInterface mediumInterfaceSmoke(&(*smokeMed.med), nullptr);
		// Feimos::getMediumBox(smoke2World, pMin, pMax, prims, nullptr, mediumInterfaceSmoke);
	}

	{
		emit PrintString("Init Dragon Mesh...");
		Feimos::Transform tri_Object2WorldStart;
		Feimos::Transform tri_Object2WorldEnd = Feimos::Translate(Feimos::Vector3f(0.6f, 0.35f, 0.5f));
		Feimos::AnimatedTransform animatedTrans(&tri_Object2WorldStart, 0.0f, &tri_Object2WorldEnd, 1.0f);
		Feimos::Transform tri_Object2World = Feimos::Translate(Feimos::Vector3f(0.f, -0.9f, 0.5f)) * Feimos::RotateY(0) * Feimos::Scale(0.5, 0.5, 0.5);
		Feimos::Spectrum dragonColor;
		dragonColor[0] = 1.0;
		dragonColor[1] = 1.0;
		dragonColor[2] = 0.0;
		std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> KdDragon = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(dragonColor);
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
		std::shared_ptr<Feimos::Material> dragonMaterial = getMetalMaterial(etaM, kM);
		Feimos::getMovingDragon(animatedTrans, tri_Object2World, dragonMaterial, prims, noMedium);

		// Feimos::Transform tri_Box2World = Feimos::Translate(Feimos::Vector3f(-0.3f, -0.9f, -1.0f));
		// Feimos::getMovingBox(animatedTrans, tri_Box2World, 1.5, 1.5, 1.5, prims, dragonMaterial, noMedium);
	}

	{
		// emit PrintString("Init Model");
		// Feimos::Transform tri_Object2WorldModel;
		// std::vector<Feimos::Transform> modelTrans;
		// std::vector<Feimos::AnimatedTransform> modelAnimatedTrans;
		// // Feimos::Transform tri_Object2WorldModel = Feimos::Translate(Feimos::Vector3f(0.0, -2.7, 1.0)) * Feimos::Scale(0.9, 0.9, 0.9);
		// // Feimos::ModelLoad ML;
		// // ML.loadModel("../../Resources/Tree/Tree.obj", tri_Object2WorldModel);
		// // ML.buildTextureModel(tri_Object2WorldModel, nullptr, prims);

		// // Feimos::Transform tri_Object2WorldModel = Feimos::Translate(Feimos::Vector3f(0.0f, -2.5f, 0.0f)) * Feimos::Scale(1.003f, 1.003f, 1.003f);
		// // Feimos::RattlerLoad rattler("../../Resources/Rattler/Tree-Rattler/", tri_Object2WorldModel, prims, noMedium);

		// // Feimos::Transform tri_Object2WorldModel = Feimos::Translate(Feimos::Vector3f(0.0f, -3.5f, 0.0f)) * Feimos::Scale(0.3f, 0.3f, 0.3f);
		// // Feimos::RattlerLoad rattler("../../Resources/Rattler/Nanosuit-Rattler/", tri_Object2WorldModel, prims, noMedium);

		// // Feimos::Transform tri_Object2WorldModel = Feimos::Translate(Feimos::Vector3f(0.0f, -3.5f, 0.0f)) * Feimos::Scale(0.003f, 0.003f, 0.003f);
		// // Feimos::RattlerLoad rattler("../../Resources/Rattler/Girl-Rattler/", tri_Object2WorldModel, prims, noMedium);

		// modelTrans.push_back(Feimos::Translate(Feimos::Vector3f(0.0, 1.2, 0.0)));
		// modelTrans.push_back(Feimos::Translate(Feimos::Vector3f(0.0, -1.2, 0.0)));
		// modelTrans.push_back(Feimos::Translate(Feimos::Vector3f(1.2, 0.0, 0.0)));
		// modelTrans.push_back(Feimos::Translate(Feimos::Vector3f(-1.2, 0.0, 0.0)));
		// modelTrans.push_back(Feimos::Translate(Feimos::Vector3f(0.0, 0.0, 0.0)));
		// modelTrans.push_back(Feimos::Translate(Feimos::Vector3f(0.0, 0.0, -1.6)));
		// for (int i = 0; i < 6; i++)
		// {
		// 	modelAnimatedTrans.push_back(Feimos::AnimatedTransform(&modelTrans[i], 0.0, &modelTrans[i], 1.0));
		// }
		// // tri_Object2WorldModel = Feimos::Translate(Feimos::Vector3f(0.0f, -0.8, 0.0f)) * Feimos::RotateX(-90) * Feimos::Scale(0.05, 0.05, 0.05);
		// // Feimos::RattlerLoad rattler("../../Resources/Rattler/Skull-Rattler/", tri_Object2WorldModel, prims, noMedium, modelAnimatedTrans, true);
		// tri_Object2WorldModel = Feimos::Translate(Feimos::Vector3f(0.0f, -3.5f, 0.0f)) * Feimos::Scale(0.3f, 0.3f, 0.3f);
		// Feimos::RattlerLoad rattler("../../Resources/Rattler/Nanosuit-Rattler/", tri_Object2WorldModel, prims, noMedium, modelAnimatedTrans, true);

		// emit PrintDataD("  MeshNumInModel: ", rattler.MeshNumInModel);
		// //  for (int i = 0; i < rattler.meshFiles.size(); i++) emit PrintString(rattler.meshFiles[i].c_str());
		// // emit PrintDataD("rattler.meshFiles.size(): ", rattler.meshFiles.size());

		// if (rattler.modelReadFlag == false)
		// 	emit PrintString("Fail to Open Model...");
	}

	// 光源
	std::vector<std::shared_ptr<Feimos::Light>> lights;

	{ // 面光源
		emit PrintString("Init AreaLight...");
		Feimos::Transform tri_Object2World_AreaLight = Feimos::Translate(Feimos::Vector3f(0.0f, 1.99f, 0.0f));
		std::shared_ptr<Feimos::Material> areaLightMaterial = Feimos::getMatteMaterial();
		Feimos::Spectrum power(5.f);
		Feimos::getAreaLight(tri_Object2World_AreaLight, lights, prims, noMedium, areaLightMaterial, power);
	}

	{ // 无限环境光源
	  // emit PrintString("Init InfiniteLight...");
	  // Feimos::Transform InfinityLightToWorld = Feimos::RotateX(20) * Feimos::RotateY(-90) * Feimos::RotateX(90);
	  // Feimos::getInfiniteLight("../../Resources/MonValley1000.hdr", lights, InfinityLightToWorld);
	}

	// 生成加速结构
	emit PrintString("Init Accelerator...");
	std::shared_ptr<Feimos::Aggregate> aggregate;
	{
		aggregate = std::make_unique<Feimos::BVHAccel>(prims, 1);
	}

	// 生成采样器结构
	emit PrintString("Init Sampler...");
	Feimos::Bounds2i ScreenBound(Feimos::Point2i(0, 0), Feimos::Point2i(WIDTH, HEIGHT));
	std::shared_ptr<Feimos::ClockRandSampler> sampler;
	{
		sampler = std::make_unique<Feimos::ClockRandSampler>(8, ScreenBound);
	}

	// 生成场景
	emit PrintString("Build Scene...");
	std::unique_ptr<Feimos::Scene> worldScene;
	{
		worldScene = std::make_unique<Feimos::Scene>(aggregate, lights);
	}

	emit PrintString("Build Integrator...");
	std::shared_ptr<Feimos::Integrator> integrator;
	{
		integrator = std::make_shared<Feimos::PathIntegrator>(15, camera, sampler, ScreenBound, 1.f, "spatial", p_framebuffer);
		// integrator = std::make_shared<Feimos::WhittedIntegrator>(15, camera, sampler, ScreenBound, p_framebuffer);
		// integrator = std::make_shared<Feimos::VolPathIntegrator>(15, camera, sampler, ScreenBound, 1.f, "spatial", p_framebuffer);
	}

	emit PrintString("Start Rendering!");
	// 开始执行渲染
	int renderCount = 0;
	double wholeTime = 0.0;
	while (renderFlag)
	{
		QElapsedTimer t;
		t.start();

		double frameTime;
		integrator->Render(*worldScene, frameTime);

#if windows_operating_system
		{
			renderCount++;
			wholeTime += frameTime;
			m_RenderStatus.setDataChanged("Performance", "One Frame Time", QString::number(frameTime), "");
			m_RenderStatus.setDataChanged("Performance", "Frame pre second", QString::number(1.0f / (float)frameTime), "");
			m_RenderStatus.setDataChanged("Performance", "Samples pre frame", QString::number(renderCount), "");
			m_RenderStatus.setDataChanged("Performance", "Whole time", QString::number(wholeTime), "seconds");
		}
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
