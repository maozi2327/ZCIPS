#pragma once

#include <QDialog>
#include "ui_linedetairdisposedialog.h"
#include "addmodifyairdisposedialog.h"
#include "../Public/util/logmacro.h"
#include "../Public/headers/setupdata.h"
#include <memory>
#include "../Public/util/messagebox.h"

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

public:
	LineDetAirDisposeDialog(const QString& _orgPath, QWidget *parent = Q_NULLPTR);
	~LineDetAirDisposeDialog();

private:
	Ui::LineDetAirDisposeDialog ui;
	AddModifyAirDisposeDialog* d_newAirDialog;
	std::unique_ptr<LineDetScanInterface> d_airTuneScan;
	QString d_templateFileName;
	bool loadFileInfo();
	bool saveTemplateData();
	bool d_modified;
	std::vector<std::vector<QString>> d_fileInfo;
	std::vector<AirTemplateData> d_airTemplateData;
	std::vector<AirTemplateData>::iterator d_currentTempDataIter;
	QString d_airFileDirectory;
public slots:
	void updateScanButtonSlot(bool _sts);

signals:
	void updateScanButtonSignal(bool _sts);

private slots:
	void on_addButton_clicked();
	void on_AirDisposeNameListWidget_currentRowChanged(int _currentRow);

protected:
	virtual void showEvent(QShowEvent* _event) override;
	virtual void closeEvent(QCloseEvent* _event) override;

private:

signals:
	LOGSIGNAL
	
};
