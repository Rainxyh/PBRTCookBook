#include "RenderThread.h"
#include "DebugText.hpp"
#include <QElapsedTimer>

#include "Core/FeimosRender.h"
#include "Shape/Triangle.h"
#include "Shape/plyRead.h"
#include "Core/primitive.h"
#include "Accelerator/BVHAccel.h"
#include "Core/interaction.h"

#include "RenderStatus.h"
#include "Shape/Sphere.h"
#include <stdlib.h>
#include <time.h>
#include <omp.h>

inline void ClockRandomInit()
{
	srand((unsigned)time(NULL));
}
inline double getClockRandom()
{
	return rand() / (RAND_MAX + 1.0);
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
	emit PrintString("Prepared to Render");

	ClockRandomInit();

	int WIDTH = 500;
	int HEIGHT = 500;

	p_framebuffer->bufferResize(WIDTH, HEIGHT);

	// 相机参数初始化：光追三部曲的风格
	Feimos::Vector3f lower_left_corner(-2.0, -2.0, -2.0);
	Feimos::Vector3f horizontal(4.0, 0.0, 0.0);
	Feimos::Vector3f vertical(0.0, 4.0, 0.0);
	Feimos::Point3f origin(0.0, 0.0, -4.0);

	// Feimos::Transform sphereT_Object2World, sphereT_World2Object;
	// Feimos::Shape* s = new Feimos::Sphere(&sphereT_Object2World, &sphereT_World2Object, false, 2.0);

	// ********************************************************生成Mesh加速结构********************************************************
	// 构建变换矩阵
	Feimos::Transform tri_Object2World, tri_World2Object;
	tri_Object2World = Feimos::Translate(Feimos::Vector3f(0.0, -2.5, 0.0)) * tri_Object2World;
	tri_World2Object = Inverse(tri_Object2World);
	// 加载模型
	Feimos::plyInfo *plyi;
	plyi = new Feimos::plyInfo("../../Resources/dragon.3d");
	// 生成网格
	std::shared_ptr<Feimos::TriangleMesh> mesh;
	mesh = std::make_shared<Feimos::TriangleMesh>(tri_Object2World, plyi->nTriangles, plyi->vertexIndices, plyi->nVertices, plyi->vertexArray, nullptr, nullptr, nullptr, nullptr);
	// 构建三角形列表
	std::vector<std::shared_ptr<Feimos::Shape>> tris;
	tris.reserve(plyi->nTriangles);
	for (int i = 0; i < plyi->nTriangles; ++i)
		tris.push_back(std::make_shared<Feimos::Triangle>(&tri_Object2World, &tri_World2Object, false, mesh, i));
	// 三角形转换为Primitive
	std::vector<std::shared_ptr<Feimos::Primitive>> prims;
	for (int i = 0; i < plyi->nTriangles; ++i)
		prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(tris[i]));
	// 用Primitive构建加速结构
	Feimos::Aggregate *agg;
	agg = new Feimos::BVHAccel(prims, 1);
	// ********************************************************生成Mesh加速结构********************************************************

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

#pragma omp parallel for
		for (int i = 0; i < WIDTH; i++)
		{
			for (int j = 0; j < HEIGHT; j++)
			{

				float u = float(i + getClockRandom()) / float(WIDTH);
				float v = float(j + getClockRandom()) / float(HEIGHT);
				int offset = (WIDTH * j + i);

				Feimos::Ray r(origin, (lower_left_corner + u * horizontal + v * vertical) - Feimos::Vector3f(origin));
				Feimos::SurfaceInteraction isect;
				Feimos::Vector3f colObj(1.0, 1.0, 1.0);
				if (agg->Intersect(r, &isect))
				{
					colObj = Feimos::Vector3f(1.0, 0.0, 0.0);
				}

				p_framebuffer->update_f_u_c(i, HEIGHT - j - 1, 0, renderCount, colObj.x);
				p_framebuffer->update_f_u_c(i, HEIGHT - j - 1, 1, renderCount, colObj.y);
				p_framebuffer->update_f_u_c(i, HEIGHT - j - 1, 2, renderCount, colObj.z);
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
}
