#pragma once
#ifndef __RenderStatus_h__
#define __RenderStatus_h__

#include <QObject>

class renderStatus : public QObject {
	Q_OBJECT

public:
	renderStatus(QObject * parent = Q_NULLPTR) { }
	~renderStatus() {};


	//Çå³ýÈ«²¿
	void clearAllStatus() {
		
	}

private:

public:


signals:
	void setDataChanged(const QString& Group, const QString& Name, const QString& Value, const QString& Unit = "", const QString& Icon = "");

private slots:


};


extern renderStatus m_RenderStatus;





#endif











