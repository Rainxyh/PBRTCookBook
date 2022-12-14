#include "RenderThread.h"
#include "DebugText.hpp"
#include <QElapsedTimer>
#include <omp.h>

#include "Core/FeimosRender.h"

#include "Core/TimeClockRandom.h"
#include "RenderStatus.h"

#define windows_operating_system false
#if windows_operating_system
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
void showMemoryInfo(void)
{

	//  SIZE_T PeakWorkingSetSize; //��ֵ�ڴ�ʹ��
	//  SIZE_T WorkingSetSize; //�ڴ�ʹ��
	//  SIZE_T PagefileUsage; //�����ڴ�ʹ��
	//  SIZE_T PeakPagefileUsage; //��ֵ�����ڴ�ʹ��

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

	emit PrintString("Start Rendering!");
	// ��ʼִ����Ⱦ
	int renderCount = 0;
	double wholeTime = 0.0;
	while (renderFlag)
	{
		QElapsedTimer t;
		t.start();

		p_framebuffer->renderCountIncrease();

		double start = omp_get_wtime(); // ��ȡ��ʼʱ��
		for (int i = 0; i < WIDTH; i++)
		{
			for (int j = 0; j < HEIGHT; j++)
			{

				p_framebuffer->update_f_u_c(i, j, 0, (float)i / (float)WIDTH);
				p_framebuffer->update_f_u_c(i, j, 1, 1.0);
				p_framebuffer->update_f_u_c(i, j, 2, (float)j / (float)HEIGHT);
				p_framebuffer->set_uc(i, HEIGHT - j - 1, 3, 255);
			}
		}

		// ���㲢��ʾʱ��
		double end = omp_get_wtime();
		double timeConsume = end - start;

#if windows_operating_system
		{
			renderCount++;
			wholeTime += timeConsume;
			m_RenderStatus.setDataChanged("Performance", "One Frame Time", QString::number(timeConsume), "");
			m_RenderStatus.setDataChanged("Performance", "Frame pre second", QString::number(1.0f / (float)timeConsume), "");
			m_RenderStatus.setDataChanged("Performance", "Samples pre frame", QString::number(renderCount), "");
			m_RenderStatus.setDataChanged("Performance", "Whole time", QString::number(wholeTime), "seconds");
		}
#endif

		emit PaintBuffer(p_framebuffer->getUCbuffer(), WIDTH, HEIGHT, 4);

		while (t.elapsed() < 100)
			;

#if windows_operating_system
		showMemoryInfo();
#endif
	}

	emit PrintString("End Rendering.");
}
