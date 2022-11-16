#ifndef __ImageToolsDockWidget_h__
#define __ImageToolsDockWidget_h__

#include <QDockWidget>
#include <QFrame>
#include <QVBoxLayout>
#include <QPushButton>

#include "DataTreeWidget.h"

class InteractionDockWidget : public QDockWidget {
	Q_OBJECT

public:
	InteractionDockWidget(QWidget * parent = Q_NULLPTR);
	~InteractionDockWidget();


private:
	void setupDock();

private:
	QVBoxLayout *centerLayout;
	QFrame *dockCentralWidget;

public:
	QPushButton *renderButton;
	DataTreeWidget * m_DataTreeWidget;

protected:
	void closeEvent(QCloseEvent *event);
	
private slots :
	
	
};




	






	
#endif







