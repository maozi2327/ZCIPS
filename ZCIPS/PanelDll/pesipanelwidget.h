#pragma once

#include <QWidget>
#include "ui_pesipanelwidget.h"

class PEsiPanelWidget : public QWidget
{
	Q_OBJECT

public:
	PEsiPanelWidget(QWidget *parent = Q_NULLPTR);
	~PEsiPanelWidget();


private:
	Ui::PEsiPanelWidget ui;

private slots:
	void on_cycleTimeEdit_returnPressed();
	void on_gainComboBox_currentIndexChanged(const QString& _text);

signals:
	void setCycleTimeSignal(int _cycleTime);
	void setGainFactorSignal(QString _text);
};
