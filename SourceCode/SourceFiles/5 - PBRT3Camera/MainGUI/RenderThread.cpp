#include "RenderThread.h"
#include "DebugText.hpp"
#include <QElapsedTimer>

#include "Core/FeimosRender.h"
#include "Core/primitive.h"
#include "Core/Spectrum.h"
#include "Core/interaction.h"

#include "Shape/Triangle.h"
#include "Shape/plyRead.h"

#include "Accelerator/BVHAccel.h"

#include "Camera/Camera.h"
#include "Camera/Perspective.h"
#include "Camera/orthographic.h"

#include "Sampler/TimeClockRandom.h"

#include "RenderStatus.h"

#include <omp.h>

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
	//初始化程序
	Feimos::Point3f eye(-3.0f, 1.5f, -3.0f), look(0.0, 0.0, 0.0f);
	Feimos::Vector3f up(0.0f, 1.0f, 0.0f);
	Feimos::Transform lookat = LookAt(eye, look, up);
	//取逆是因为LookAt返回的是世界坐标到相机坐标系的变换
	//而我们需要相机坐标系到世界坐标系的变换
	Feimos::Transform Camera2World = Inverse(lookat);
	Feimos::Camera *cam = nullptr;
	cam = Feimos::CreatePerspectiveCamera(WIDTH, HEIGHT, Camera2World);
	cam = Feimos::CreateOrthographicCamera(WIDTH, HEIGHT, Camera2World);

	// 生成Mesh加速结构
	std::shared_ptr<Feimos::TriangleMesh> mesh;
	std::vector<std::shared_ptr<Feimos::Shape>> tris;
	std::vector<std::shared_ptr<Feimos::Primitive>> prims;
	Feimos::plyInfo *plyi;
	Feimos::Aggregate *agg;
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
	agg = new Feimos::BVHAccel(prims, 1);

	emit PrintString("Start Rendering");
	// 开始执行渲染
	int renderCount = 0;
	while (renderFlag)
	{
		QElapsedTimer t;
		t.start();

		omp_set_num_threads(32);		//设置线程的个数
		double start = omp_get_wtime(); //获取起始时间

		// emit PrintString("Rendering");
		renderCount++;

		Feimos::Vector3f Light(1.0, 1.0, 1.0);
		Light = Normalize(Light);

#pragma omp parallel for
		for (int i = 0; i < WIDTH; i++)
		{
			for (int j = 0; j < HEIGHT; j++)
			{

				float u = float(i + getClockRandom()) / float(WIDTH);
				float v = float(j + getClockRandom()) / float(HEIGHT);
				int offset = (WIDTH * j + i);

				Feimos::CameraSample cs;
				cs.pFilm = Feimos::Point2f(i + getClockRandom(), j + getClockRandom());
				cs.pLens = Feimos::Point2f(getClockRandom(), getClockRandom());
				Feimos::Ray r;
				// per_cam->GenerateRay(cs, &r);
				cam->GenerateRay(cs, &r);

				Feimos::SurfaceInteraction isect;
				Feimos::Spectrum colObj(0.0f);
				if (agg->Intersect(r, &isect))
				{
					float Li = Feimos::Dot(Light, isect.n);
					colObj[1] = std::abs(Li); //取绝对值，防止出现负值
				}

				p_framebuffer->update_f_u_c(i, j, 0, renderCount, colObj[0]);
				p_framebuffer->update_f_u_c(i, j, 1, renderCount, colObj[1]);
				p_framebuffer->update_f_u_c(i, j, 2, renderCount, colObj[2]);
				p_framebuffer->set_uc(i, HEIGHT - j - 1, 3, 255);
			}
		}

		// 计算并显示时间
		double end = omp_get_wtime();
		double frameTime = end - start;
		m_RenderStatus.setDataChanged("Performance", "One Frame Time", QString::number(frameTime), "");
		m_RenderStatus.setDataChanged("Performance", "Frame pre second", QString::number(1.0f / (float)frameTime), "");

		emit PaintBuffer(p_framebuffer->getUCbuffer(), WIDTH, HEIGHT, 4);

		while (t.elapsed() < 1)
			;
	}

	emit PrintString("End Rendering");
}
