#pragma once

#ifndef __FrameBuffer_H__
#define __FrameBuffer_H__
#include "MainGUI\DebugText.hpp"

#include <QObject>


class FrameBuffer : public QObject {
	Q_OBJECT

public:
	FrameBuffer() :
		width(width), 
		height(height),
		channals(channals),
		ubuffer(nullptr) { }

	~FrameBuffer(){
		if (nullptr != ubuffer) delete[] ubuffer;
		ubuffer = nullptr;
		this->width = 0;
		this->height = 0;
		this->channals = 0;
	}

	void InitBuffer(const int width = 800, const int height = 600, const int channals = 4) {
		this->width = width;
		this->height = height;
		this->channals = channals;
		ubuffer = nullptr;
		if (channals > 4) {
			TextDinodonS("Initialize Error: Channel is greater than 4!");
			return;
		}
		ubuffer = new unsigned char[width * height * channals];
		if (nullptr == ubuffer) {
			TextDinodonS("Initialize Error: FrameBuffer has not applied for enough memory!");
			this->width = 0;
			this->height = 0;
			this->channals = 0;
		}
	}
	void FreeBuffer() {
		if (nullptr != ubuffer) delete[] ubuffer;
		ubuffer = nullptr;
		this->width = 0;
		this->height = 0;
		this->channals = 0;
	}
	bool bufferResize(const int width = 800, const int height = 600) {
		if (this->width == 0 || this->height == 0 || this->channals == 0) {
			TextDinodonS("Resize Error: The previous buffer size was 0 !");
			return false;
		}
		if (nullptr != ubuffer) delete[] ubuffer;
		this->width = width; this->height = width;
		ubuffer = new unsigned char[width * height * channals];
		if (nullptr == ubuffer) {
			TextDinodonS("Resize Error: FrameBuffer has not applied for enough memory!");
			this->width = 0;
			this->height = 0;
			this->channals = 0;
			return false;
		}
		return true;
	}

	inline bool set_uc(const int w, const int h, const int shifting, const unsigned char & dat) {
		if (nullptr == ubuffer) {
			TextDinodonS("Access Error: Buffer is empty and cannot be accessed!");
			return false;
		}
		if (w >= width || h >= height || w < 0 || h < 0) {
			TextDinodonS("Access Error: Coordinates exceed array limit!");
			return false;
		}
		int offset = (w + h * width) * channals + shifting;
		ubuffer[offset] = dat;
		return true;
	}

	unsigned char * getUCbuffer() { return ubuffer; }

private:
	unsigned char * ubuffer;
	int width;
	int height;
	int channals;

};





#endif





