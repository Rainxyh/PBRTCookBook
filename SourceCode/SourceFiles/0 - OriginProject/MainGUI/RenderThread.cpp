#include "RenderThread.h"
#include "DebugText.hpp"
#include <QTime>

RenderThread::RenderThread() {
	paintFlag = false;
	renderFlag = false;
}

RenderThread::~RenderThread() {
	
}

void RenderThread::run() {
	emit PrintString("Prepared to Render");


	// ¿ªÊ¼Ö´ÐÐäÖÈ¾
	while (renderFlag) {
		QTime t;
		t.start();
		
		emit PrintString("Rendering");

		for (int i = 0; i < 800; i++) {
			for(int j = 0; j < 600; j++) {
				p_framebuffer->set_uc(i, j, 0, 255);
				p_framebuffer->set_uc(i, j, 1, 0);
				p_framebuffer->set_uc(i, j, 2, 0);
				p_framebuffer->set_uc(i, j, 3, 255);
			}
		}

		
		emit PaintBuffer(p_framebuffer->getUCbuffer(), 800, 600, 4);
			
		while (t.elapsed() < 20);
	}
	
}











