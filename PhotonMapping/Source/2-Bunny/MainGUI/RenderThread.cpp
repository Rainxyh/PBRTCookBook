#include "RenderThread.h"
#include "DebugText.hpp"
#include <QTime>
#include <omp.h>

#include "Core\FeimosRender.h"

#include "Core\TimeClockRandom.h"
#include "RenderStatus.h"

void showMemoryInfo(void);

#include "Core\bvhTree.h"
#include "Core\texture.h"
#include "Core\Material.h"
#include "Core\Box.h"
#include "Core\Triangle.h"
#include "Core\change.h"
#include "Core\Sphere.h"
#include "Core\Camera.h"
#include "Core\readOffFile.h"

material *light = new diffuse_light(new constant_texture(vec3(27.0f, 27.0f, 27.0f)));
hitable *light_shape = (new yz_rect(1.13, 2.43, 2.27, 3.32, 0.01, light));
hitable *cornell_box() {
	hitable **list = new hitable*[100];
	int index = 0;
	material *red = new lambertian(new constant_texture(vec3(0.45, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.53, 0.53, 0.53)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	
	material *bunnymtrl = new dielectric(1.5, vec3(0.87, 0.49, 0.173));
	material *glass = new dielectric(1.5);

	//右左墙
	list[index++] = light_shape;
	list[index++] = new flip_normals(new yz_rect(0, 5.55, 0, 5.55, 5.55, red));
	list[index++] = new yz_rect(0, 5.55, 0, 5.55, 0, green);
	//天花板 地板
	list[index++] = new flip_normals(new xz_rect(0, 5.55, 0, 5.55, 5.55, white));
	list[index++] = new xz_rect(0, 5.55, 0, 5.55, 0, white);
	//前墙
	list[index++] = new flip_normals(new xy_rect(0, 5.55, 0, 5.55, 5.55, white));

	hitable *b1 = new translate(new rotate_y(new box(vec3(0, 0.0, 0), vec3(0.4, 0.4, 0.4), white), -18), vec3(3.90, 0, 1.85));
	list[index++] = b1;
	
	//list[index++] = new sphere(vec3(1.5,1.5,1.5), 0.5, glass);

	offRead * myOffRead = new offRead(bunnymtrl);
	hitable *b4 = myOffRead->m_bvh;
	list[index++] = b4;

	return new bvh_node(list, index, 0.0, 1.0);
}

hitable *world = cornell_box();


vec3 color(const Ray&r, hitable *world, hitable * light_shape, int depth) {
	hit_record hrec;
	if (world->hit(r, 0.0001, FLT_MAX, hrec)) {
		scatter_record srec;
		vec3 emitted = hrec.mat_ptr->emitted(r, hrec, hrec.texU, hrec.texV, hrec.p);
		//if(emitted.squaredLength()!=0) return vec3(1.0, 1.0, 1.0);
		if (depth < 16 && hrec.mat_ptr->scatter(r, hrec, srec)) {
			if (srec.is_specular) {
				free(srec.pdf_ptr);
				return srec.albedo * color(srec.specular_ray, world, light_shape, depth + 1);
			}
			else {

				//非重要性随机采样
				vec3 target = hrec.p + hrec.normal + random_in_unit_sphere();
				Ray scattered = Ray(hrec.p, target - hrec.p, r.time());
				free(srec.pdf_ptr);
				return emitted + srec.albedo * color(scattered, world, light_shape, depth + 1);

				//重要性采样
				/*hitable_pdf p0(light_shape, hrec.p);
				mixture_pdf p(&p0, srec.pdf_ptr);
				Ray scattered = Ray(hrec.p, p.generate(), r.time());
				float pdf_val = p.value(scattered.direction());
				float mpdf = hrec.mat_ptr->scattering_pdf(r, hrec, scattered);
				free(srec.pdf_ptr);
				return emitted + srec.albedo * mpdf * color(scattered, world, light_shape, depth + 1) / pdf_val;*/
			}
		}
		else {
			return emitted;
		}
		free(hrec.mat_ptr);
	}
	else {
		return vec3(0, 0, 0);
	}
}


RenderThread::RenderThread() {
	paintFlag = false;
	renderFlag = false;
}

RenderThread::~RenderThread() {
	
}

void RenderThread::run() {
	emit PrintString("Prepared to Render...");

	ClockRandomInit();

	int WIDTH = 600;
	int HEIGHT = 450;

	cam.setAspt((float)WIDTH / (float)HEIGHT);
	cam.setDirection();

	emit PrintString("Init FrameBuffer...");
	p_framebuffer->bufferResize(WIDTH, HEIGHT);

	emit PrintString("Start Rendering!");
	// 开始执行渲染
	int renderCount = 0;
	double wholeTime = 0.0;
	while (renderFlag) {
		QTime t;
		t.start();
		
		p_framebuffer->renderCountIncrease();

		double start = omp_get_wtime();//获取起始时间  
#pragma omp parallel for
		for (int i = 0; i < WIDTH; i++) {
			for (int j = 0; j < HEIGHT; j++) {

				float u = (float(i) + getClockRandom()) / float(WIDTH);
				float v = (float(j) + getClockRandom()) / float(HEIGHT);
				Ray r = cam.get_ray(u, v);
				vec3 col = de_nan(color(r, world, light_shape, 0));

				col = HDRtoLDR(col, 0.85);

				p_framebuffer->update_f_u_c(i, HEIGHT - j - 1, 0, col.x());
				p_framebuffer->update_f_u_c(i, HEIGHT - j - 1, 1, col.y());
				p_framebuffer->update_f_u_c(i, HEIGHT - j - 1, 2, col.z());
				p_framebuffer->set_uc(i, HEIGHT - j - 1, 3, 255);
			}
		}

		// 计算并显示时间
		double end = omp_get_wtime();
		double timeConsume = end - start;

		{
			renderCount++;
			wholeTime += timeConsume;
			m_RenderStatus.setDataChanged("Performance", "One Frame Time", QString::number(timeConsume), "");
			m_RenderStatus.setDataChanged("Performance", "Frame pre second", QString::number(1.0f / (float)timeConsume), "");
			m_RenderStatus.setDataChanged("Performance", "Samples pre frame", QString::number(renderCount), "");
			m_RenderStatus.setDataChanged("Performance", "Whole time", QString::number(wholeTime), "seconds");
		}

		emit PaintBuffer(p_framebuffer->getUCbuffer(), WIDTH, HEIGHT, 4);
			
		while (t.elapsed() < 100);

		showMemoryInfo();
	}

	emit PrintString("End Rendering.");
	
}



#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib") 
void showMemoryInfo(void) {

	//  SIZE_T PeakWorkingSetSize; //峰值内存使用
	//  SIZE_T WorkingSetSize; //内存使用
	//  SIZE_T PagefileUsage; //虚拟内存使用
	//  SIZE_T PeakPagefileUsage; //峰值虚拟内存使用

	EmptyWorkingSet(GetCurrentProcess());

	HANDLE handle = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));

	m_RenderStatus.setDataChanged("Memory Use", "WorkingSetSize", QString::number(pmc.WorkingSetSize / 1000.f / 1000.f), "M");
	m_RenderStatus.setDataChanged("Memory Use", "PeakWorkingSetSize", QString::number(pmc.PeakWorkingSetSize / 1000.f / 1000.f), "M");
	m_RenderStatus.setDataChanged("Memory Use", "PagefileUsage", QString::number(pmc.PagefileUsage / 1000.f / 1000.f), "M");
	m_RenderStatus.setDataChanged("Memory Use", "PeakPagefileUsage", QString::number(pmc.PeakPagefileUsage / 1000.f / 1000.f), "M");

}







