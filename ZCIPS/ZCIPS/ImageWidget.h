#pragma once
#include <QWidget>
//ͼ����ʾ������
class ImageWidget : public QWidget
{
	Q_OBJECT
public:
	ImageWidget(QWidget *parent = 0);
	~ImageWidget();
protected:
	void closeEvent(QCloseEvent *event);            //���ڹر��¼�
signals:
	void ImgWidgetClose(QString strWinTitle);                          //���ڹر�ʱ�����ź�
private:

};