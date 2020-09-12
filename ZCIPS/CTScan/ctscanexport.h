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
};

//TODO_DJ_NOTEPAD
//ʹ��unique_ptr����QDialog�������Զ�����;
//ctscanwidget������Ӧcloseevent;
