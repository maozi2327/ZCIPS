#pragma once

#include <QtWidgets>
#include "ui_ctscan.h"

class CTScanWidget : public QWidget
{
    Q_OBJECT

public:
    CTScanWidget(QWidget* in_middleWidget, QWidget* in_downWidget, QWidget *parent = Q_NULLPTR);
	~CTScanWidget();

	QWidget* getWidget();
	QLayout* getLayout();
private slots:
	void on_ray0LineDet0Button_clicked();
	void on_ray0PanelDet0Button_clicked();
	void on_ray1LineDet0Button_clicked();
	void on_ray1PanelDet0Button_clicked();
	void on_showMotorWidgetButton_clicked();
private slots:
	void cut();
	void copy();
	void paste();
	void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);
protected:
#ifndef QT_NO_CONTEXTMENU
	void contextMenuEvent(QContextMenuEvent *event) override;
	void changeEvent(QEvent * event) override;
#endif // QT_NO_CONTEXTMENU
private:
    Ui::CTScanWidget ui;
	QAction *cutAct;
	QAction *copyAct;
	QAction *pasteAct;
	size_t frontImageIndex;
	QSystemTrayIcon* d_tray;
	QString	d_workDir;
	QWidget* d_scanWidget;
	QGridLayout* d_layout;
	QWidget* d_middleWidget;
	QWidget* d_downWidget;
};
//#ifdef TABLETRANSLATION
//#ifdef TABLERADIAL
//#ifdef DETLAYER
//#ifdef DETTRANSLATION
//#ifdef DETTRADIAL
//#ifdef RAYLAYER
//#ifdef DEFLECT
//#endif