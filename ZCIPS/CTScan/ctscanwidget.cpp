#include "stdafx.h"
#include "ctscanwidget.h"

CTScanWidget::CTScanWidget(QWidget* _upWidget, QWidget* _middleWidget, QWidget* _downWidget, QWidget *parent)
    : QWidget(parent)
	, d_scanWidget(nullptr), d_upWidget(_upWidget), d_scanTableWidget(_middleWidget), d_downWidget(_downWidget)
{
	ui.setupUi(this);
	d_scanTableVLayout = new QVBoxLayout;
	d_scanTableVLayout->addWidget(d_scanTableWidget, 0, 0);
	d_scanTableVLayout->setContentsMargins(0, 0, 0, 0);
	ui.scanTable->setLayout(d_scanTableVLayout);

	QGridLayout* layout1 = new QGridLayout;
	layout1->setContentsMargins(0, 0, 0, 0);
	layout1->addWidget(d_downWidget, 0, 0);
	layout1->setMargin(0);
	ui.motorControlGroupBox->setLayout(layout1);
	ui.motorControlGroupBox->setFixedHeight(d_downWidget->height());

	d_layout = new QGridLayout;
	d_layout->addWidget(this, 0, 0);
	parent->setLayout(d_layout);
	d_layout->setContentsMargins(5, 5, 5, 0);
}

CTScanWidget::~CTScanWidget()
{

}

QWidget * CTScanWidget::getWidget()
{
	return this;
}

QLayout * CTScanWidget::getLayout()
{
	return d_layout;
}

void CTScanWidget::switchLinePanelWidget(QWidget * _widget)
{
	if (d_scanTableWidget != nullptr && d_scanTableWidget->isVisible())
	{
		d_scanTableWidget->setParent(nullptr);
		d_scanTableVLayout->removeWidget(d_scanTableWidget);
	}
	d_scanTableWidget = _widget;
	_widget->setFixedHeight(_widget->height() + 2);
	d_scanTableVLayout->addWidget(d_scanTableWidget, 0, 0);
	ui.controlTabWidget->setFixedHeight(d_scanTableWidget->height() + ui.controlTabWidget->iconSize().height());
	setFixedHeight(ui.controlTabWidget->height() + ui.motorControlGroupBox->height() + 
		ui.acceleratorContainerwidget->height() + 2);
	updateGeometry();
}

void CTScanWidget::on_ray0LineDet0Button_clicked()
{
	//if (d_lineDetScanWidgetMap[{0, 0}].get() == nullptr)
	//{
	//	auto widget = new LineDetScanWidget(d_motorControl.get(), 0, 0,
	//		d_lineDetScanModeMap[{ 0, 0}], d_setupData.get(), d_lineDetNetWorkMap[0].get(), d_controller.get(), this);
	//	d_lineDetScanWidgetMap[{0, 0}].reset(widget);
	//}
	//
	//if(d_scanWidget != nullptr && d_scanWidget->isVisible())
	//	d_scanWidget->hide();

	//d_scanWidget = d_lineDetScanWidgetMap[{0, 0}].get();
	//d_scanWidget->setGeometry((QRect(0, 0, 520, 730)));
	//d_scanWidget->show();
}

void CTScanWidget::on_ray0PanelDet0Button_clicked()
{
	//if (d_panelDetScanWidget[{0, 0}].get() == nullptr)
	//{
	//	auto widget = new ConeScanWidget(d_motorControl.get(), 0, 0,
	//		d_panelDetScanModeMap[{0, 0}], d_setupData.get(), nullptr, d_controller.get(), this);
	//	d_panelDetScanWidget[{0, 0}].reset(widget);
	//}

	//if (d_scanWidget != nullptr && d_scanWidget->isVisible())
	//	d_scanWidget->hide();

	//d_scanWidget = d_panelDetScanWidget[{0, 0}].get();
	//d_scanWidget->setGeometry((QRect(0, 0, 1520, 1730)));
	//d_scanWidget->show();
}

void CTScanWidget::on_ray1LineDet0Button_clicked()
{

}

void CTScanWidget::on_ray1PanelDet0Button_clicked()
{

}

void CTScanWidget::on_showMotorWidgetButton_clicked()
{
	emit showMotorButtonSignal();
	ui.motorControlGroupBox->setVisible(!ui.motorControlGroupBox->isVisible());
}

void CTScanWidget::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) 
	{
	case QSystemTrayIcon::Trigger:
		break;
	case QSystemTrayIcon::DoubleClick:
		this->showNormal();
		break;
	default:
		break;
	}
}

void CTScanWidget::cut()
{
	hide();                 /* ��С�������� */
	d_tray->show();
}

void CTScanWidget::copy()
{
	emit switchToLineWidgetSignal(0, 0);
}

void CTScanWidget::paste()
{
	emit switchToPanelWidgetSignal(0, 0);
}

void CTScanWidget::contextMenuEvent(QContextMenuEvent *event)
{
	cutAct = new QAction(tr("Cu&t"), this);
	cutAct->setShortcuts(QKeySequence::Cut);
	cutAct->setStatusTip(tr("Cut the current selection's contents to the "
		"clipboard"));

	connect(cutAct, &QAction::triggered, this, &CTScanWidget::cut);

	copyAct = new QAction(tr("&Copy"), this);
	copyAct->setShortcuts(QKeySequence::Copy);
	copyAct->setStatusTip(tr("Copy the current selection's contents to the "
		"clipboard"));
	connect(copyAct, &QAction::triggered, this, &CTScanWidget::copy);

	pasteAct = new QAction(tr("&Paste"), this);
	pasteAct->setShortcuts(QKeySequence::Paste);
	pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
		"selection"));
	connect(pasteAct, &QAction::triggered, this, &CTScanWidget::paste);
	QMenu menu(this);
	menu.addAction(cutAct);
	menu.addAction(copyAct);
	menu.addAction(pasteAct);
	menu.exec(event->globalPos());
}

void CTScanWidget::changeEvent(QEvent * event)
{
	if ((event->type() == QEvent::WindowStateChange) && isMinimized())
	{
		hide();
		d_tray->show();
	}
}

void CTScanWidget::closeEvent(QCloseEvent * event)
{

}

