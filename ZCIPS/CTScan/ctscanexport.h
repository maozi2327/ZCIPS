#pragma once
#include "ctscan_global.h"
#include <QObject>

class CTSCANDLL_EXPORT CTScanInterface : public QObject
{
	Q_OBJECT
public slots:
	void slot1(int i);

signals:
	void signal1(bool);

protected:
	CTScanInterface();

public:
	static CTScanInterface* loadCtScanInstance(QWidget* _parent);
	~CTScanInterface();
	virtual void ctScanWidgetClosed();
	virtual QMenuBar* getMenuBar() = 0;
};

//TODO_DJ_NOTEPAD
//使用unique_ptr包裹QDialog，不会自动析构;
//ctscanwidget不能响应closeevent;
