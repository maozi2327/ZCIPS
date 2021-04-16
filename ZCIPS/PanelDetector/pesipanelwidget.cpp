#include "stdafx.h"
#include "pesipanelwidget.h"

PEsiPanelWidget::PEsiPanelWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.cycleTimeEdit->setText("500");
	QStringList gainGroupItem{"0.25pF", "0.5pF", "1pF", "2pF", "4pF", "8pF"};
	ui.gainComboBox->addItems(gainGroupItem);
}
PEsiPanelWidget::~PEsiPanelWidget()
{
}

void PEsiPanelWidget::on_cycleTimeEdit_returnPressed()
{
	int time = ui.cycleTimeEdit->text().toInt();
	emit setCycleTimeSignal(time);
}

void PEsiPanelWidget::on_gainComboBox_currentIndexChanged(const QString& _text)
{
	emit setGainFactorSignal(_text);
}
