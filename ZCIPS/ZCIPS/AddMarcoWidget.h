#pragma once

#include <QtWidgets>
#include "MarcoPipeArea.h"
#include "Marco.h"
#include "ImageInstance.h"
#include "ERRInfo.h"

class AddMarcoWidget : public QWidget
{
	Q_OBJECT

// 重载与响应
public:
	AddMarcoWidget
	(
		QSharedPointer<Marco> pInMarco,
		QSharedPointer<ImageInstance> pInImageIns,
		QWidget* parent = nullptr
	);

// 参数
public:
	QSharedPointer<Marco> pMarco;
	QSharedPointer<ImageInstance> pImageIns;

// 控件
public:
	QGridLayout *pLayout;        // 布局器
	QToolButton *pBtnAdd;        // 增加按钮
	MarcoPipeArea *pPipArea;     // 处理管道
	QLineEdit *pEdit;            // 编辑控件
	QLabel *pLabel;              // 文字显示控件 

// 函数
public:

	// 界面初始化
	void InitUI();

	// 加载处理管道信息
	void LoadProPipeInfo();

// 槽函数
public slots:

	// 生成函数
	void GenerateSlot();

};