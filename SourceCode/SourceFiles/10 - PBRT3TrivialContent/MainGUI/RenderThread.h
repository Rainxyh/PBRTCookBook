#ifndef __RenderThread_h__
#define __RenderThread_h__

#include <QThread>
#include <QString>

#include "Core\FrameBuffer.h"
#include "IMAGraphicsView.h"

class RenderThread :public QThread {

	Q_OBJECT
public:
	RenderThread();
	~RenderThread();

public:
	bool renderFlag;
	bool paintFlag;
	FrameBuffer* p_framebuffer;

signals:
	void PrintString(char* s);
	void PrintDataD(const char* s, const double data);
	void PaintBuffer(unsigned char* buffer, int width, int height, int channals);
private slots:
	


public:
	void run();


};








#endif






