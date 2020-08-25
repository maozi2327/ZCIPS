#pragma once
#include <QWidget>
//图像显示窗口类
class ImageWidget : public QWidget
{
	Q_OBJECT
public:
	ImageWidget(QWidget *parent = 0);
	~ImageWidget();
protected:
	void closeEvent(QCloseEvent *event);            //窗口关闭事件
signals:
	void ImgWidgetClose(QString strWinTitle);                          //窗口关闭时发出信号
private:

};