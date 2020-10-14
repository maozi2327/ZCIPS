#pragma once

#include <QDialog>
#include "ui_linedetairdisposedialog.h"
#include "addmodifyairdisposedialog.h"
#include "../Public/util/logmacro.h"
#include "../Public/headers/setupdata.h"
#include <memory>

class LineDetScanInterface;
class ControllerInterface;
class LineDetNetWork;

struct AirTemplateData
{
	QString Name;
	float Voltage;
	float Current;
	QString Remark;
};

class LineDetAirDisposeDialog : public QDialog
{
	Q_OBJECT

protected:
	LineDetAirDisposeDialog(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWork,
		const SetupData* _setupData, int _lineDetIndex, int _rayIndex, const QString& _orgPath, QWidget *parent = Q_NULLPTR);
public:	
	~LineDetAirDisposeDialog();

private:
	Ui::LineDetAirDisposeDialog ui;
	AddModifyAirDisposeDialog* d_newAirDialog;
	std::unique_ptr<LineDetScanInterface> d_airTuneScan;
	QString d_templateFileName;
	bool loadTemplateData();
	bool saveTemplateData();
	std::vector<AirTemplateData> d_airTemplateData;
	std::vector<AirTemplateData>::iterator d_currentTempDataIter;
public slots:
	void updateScanButtonSlot(bool _sts);

signals:
	void updateScanButtonSignal(bool _sts);

private slots:
	void on_addButton_clicked();
	void on_AirDisposeNameListWidget_currentRowChanged(int _currentRow);

protected:
	virtual void showEvent(QShowEvent* _event) override;
signals:
	LOGSIGNAL

public:
	static LineDetAirDisposeDialog* getInstance(ControllerInterface* _controller,
		LineDetNetWork* _lineDetNetWork,
		const SetupData* _setupData, int _lineDetIndex, int _rayIndex, 
		const QString& _orgPath, QWidget *parent)
	{
		static std::unique_ptr<LineDetAirDisposeDialog> handle(new LineDetAirDisposeDialog(_controller, _lineDetNetWork,
			_setupData, _lineDetIndex, _rayIndex,
			_orgPath, nullptr));
		return handle.get();
	};
};
