#pragma once

#include <QtWidgets>
#include "ui_ctscanwidget.h"
//#include <QCloseEvent>
class CTScanWidget : public QWidget
{
    Q_OBJECT

public:
    CTScanWidget(QWidget* _upWidget, QWidget* _middleWidget, QWidget* _downWidget, QWidget *parent = Q_NULLPTR);
	~CTScanWidget();

	QWidget* getWidget();
	QLayout* getLayout();
	void switchLinePanelWidget(QWidget* _widget);
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
signals:
	void ctScanWidgetClosed();
	void showMotorButtonSignal();
	void switchToPanelWidgetSignal(int _rayId, int detId);
	void switchToLineWidgetSignal(int _rayId, int detId);
protected:
#ifndef QT_NO_CONTEXTMENU
	void contextMenuEvent(QContextMenuEvent *event) override;
	void changeEvent(QEvent * event) override;
	void closeEvent(QCloseEvent *event) override;
#endif //QT_NO_CONTEXTMENU
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
	QWidget* d_upWidget;
	QVBoxLayout* d_scanTableVLayout;
	QWidget* d_scanTableWidget;
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