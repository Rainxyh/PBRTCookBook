#include "RenderThread.h"
#include "DebugText.hpp"
#include <QElapsedTimer>

#include "RayTracer/RayTracer.h"
#include "RayTracer/camera.h"

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

	hitable *world = cornell_box();

	vec3 lookfrom(2.78, 2.78, -8.00); //
	vec3 lookat(2.78, 2.78, 0);
	float disk_to_focus = 10.0; //(lookfrom - lookat).length();
	float aperture = 0.00;
	float vfov = 40.0;
	camera cam(lookfrom, lookat, vec3(0, 1, 0), vfov, float(WIDTH) / float(HEIGHT), aperture, disk_to_focus, 0.0, 1.0);

	// ¿ªÊ¼Ö´ÐÐäÖÈ¾
	int renderCount = 0;
	while (renderFlag)
	{
		QElapsedTimer t;
		t.start();

		// emit PrintString("Rendering");
		renderCount++;

		for (int i = 0; i < WIDTH; i++)
		{
			for (int j = 0; j < HEIGHT; j++)
			{

				float u = float(i + getClockRandom()) / float(WIDTH);
				float v = float(j + getClockRandom()) / float(HEIGHT);
				int offset = (WIDTH * j + i);
				ray r = cam.get_ray(u, v);
				vec3 Li = de_nan(WhittedRT(r, world, 0));

				vec3 col = HDRtoLDR(Li, 0.8f);

				p_framebuffer->update_f_u_c(i, HEIGHT - j - 1, 0, renderCount, col.x());
				p_framebuffer->update_f_u_c(i, HEIGHT - j - 1, 1, renderCount, col.y());
				p_framebuffer->update_f_u_c(i, HEIGHT - j - 1, 2, renderCount, col.z());
				p_framebuffer->set_uc(i, HEIGHT - j - 1, 3, 255);
			}
		}

		emit PaintBuffer(p_framebuffer->getUCbuffer(), WIDTH, HEIGHT, 4);

		while (t.elapsed() < 20)
			;
	}
}
