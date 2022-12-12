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

hitable *cornell_box() {
	hitable **list = new hitable*[100000];
	int index = 0;
	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(7.0f, 7.0f, 7.0f)));
	material *mtrl = new metal(vec3(0.7, 0.7, 0.7), 0.0);
	list[index++] = new flip_normals(new yz_rect(0, 5.55, 0, 5.55, 5.55, green));
	list[index++] = new yz_rect(0, 5.55, 0, 5.55, 0, red);
	list[index++] = new flip_normals(new xz_rect(2.13, 3.43, 2.27, 3.32, 5.54, light));
	list[index++] = new flip_normals(new xz_rect(0, 5.55, 0, 5.55, 5.55, white));
	list[index++] = new xz_rect(0, 5.55, 0, 5.55, 0, white);
	list[index++] = new flip_normals(new xy_rect(0, 5.55, 0, 5.55, 5.55, white));
	//list[index++] = new sphere(vec3(4.30, 1.65 + 0.10 + 0.75, 1.25), 0.75, white);
	//list[index++] = new sphere(vec3(4.30, 1.65 + 0.10 + 0.75, 1.25), -0.40(1.35));, new dielectric
	hitable *b1 = new translate(new rotate_y(new box(vec3(0, 0.10, 0), vec3(1.65, 1.65 + 0.10, 1.65), white), 18), vec3(2.90, 0, 1.35));
	hitable *b2 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(1.65, 3.30, 1.65), mtrl), -15), vec3(0.9, 0, 2.15));
	hitable *b3 = new sphere(vec3(3.60, 0.75, 0.95), 0.70, white);
	list[index++] = b1;
	list[index++] = b2;
	return new bvh_node(list, index, 0.0, 1.0);
}

hitable *world = cornell_box();
hitable *light_shape = new xz_rect(2.13, 3.43, 2.27, 3.32, 5.54, 0);

vec3 color(const Ray&r, hitable *world, hitable * light_shape, int depth) {
	hit_record hrec;
	if (world->hit(r, 0.0001, FLT_MAX, hrec)) {
		scatter_record srec;
		vec3 emitted = hrec.mat_ptr->emitted(r, hrec, hrec.texU, hrec.texV, hrec.p);
		if (depth < 50 && hrec.mat_ptr->scatter(r, hrec, srec)) {
			if (srec.is_specular) {
				return srec.attenuation * color(srec.specular_ray, world, light_shape, depth + 1);
			}
			else {
				hitable_pdf p0(light_shape, hrec.p);
				mixture_pdf p(&p0, srec.pdf_ptr);
				Ray scattered = Ray(hrec.p, p.generate(), r.time());
				float pdf_val = p.value(scattered.direction());
				float mpdf = hrec.mat_ptr->scattering_pdf(r, hrec, scattered);
				return emitted + srec.attenuation * mpdf * color(scattered, world, light_shape, depth + 1) / pdf_val;
			}
		}
		else {
			return emitted;
		}
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
	int HEIGHT = 400;

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







