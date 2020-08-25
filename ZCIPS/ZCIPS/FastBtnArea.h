#pragma once

#include <QtWidgets>
#include "Marco.h"

/***************************** 宏 ******************************/
#define FASTBTN_UNIT_WIDTH    60    // 快速按钮宽度
#define FASTBTN_UNIT_HEIGHT   55    // 快速按钮高度
#define FASTBTN_MARGIN        10    // 显示区域边缘

/***************************** 类 ******************************/
// 快速按钮类
class FastButton : public QPushButton
{
    Q_OBJECT

// 响应与重载
public:
	FastButton(QWidget *parent = nullptr);

// 槽函数
public slots:

	// 单击事件槽函数
	void FastBtnClick(bool bIsChecked);

// 信号
signals:

	// 快捷按钮点击函数
	void FastBtn(QString StrLabel);

};

// 快速按钮区域类
class FastBtnArea : public QScrollArea
{

	Q_OBJECT

// 响应与重载
public:
	
	// 初始化
	FastBtnArea(QWidget *parent = nullptr);

// 函数
public:

	// 清除
	void Clear();

	// 快速按钮区域更新
	void FastBtnUpdate(QSharedPointer<Marco> pInMarco);

	// 返回按钮控件Vec
	QVector<FastButton *> GetBtnVec();

// 控件
private:
	QVector<FastButton *> vecFastBtn;   // 按钮指针Vec
	QGridLayout *pBtnLayout;			// 按钮Layout

};
