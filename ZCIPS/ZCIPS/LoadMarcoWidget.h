#pragma once

#include <QtWidgets>
#include "ImageInstance.h"
#include "Marco.h"
#include "FastBtnArea.h"
#include "MarcoPipeArea.h"

/*********************** 宏 **************************/
#define NO_SELECTION   -1      // 定义未选择为-1

/*********************** 枚举类型 **************************/
// 按钮类型
enum enButtonType
{
	ButtonType_Excute = 0,
	ButtonType_Delete,
	ButtonType_FastBtn
};

/*********************** 类 **************************/
// 表格按钮类
class TableButton : public QPushButton
{
	Q_OBJECT

// 响应与重载
public:

	// 构造函数
	TableButton
	(
		enButtonType BInType,
		QString StrInMarcoLabel,
		QWidget *parent = nullptr
	);

// 槽函数
public slots:
	
	// 单击槽函数
	void ClickSlot(bool bChecked);

// 信号
signals:

	// 单击信号
	void ClickEmit(enButtonType, QString);


// 参数 
private:
	enButtonType m_BType;
	QString StrMarcoLabel;

};


// 加载/编辑宏处理类
class LoadMarcoWidget : public QWidget
{
	Q_OBJECT

// 重载与响应
public:
	LoadMarcoWidget
	(
		QSharedPointer<ImageInstance> pInImageIns,
		QSharedPointer<Marco> pInMarco,
		QWidget *parent = nullptr
	);

// 参数
private:
	int iSelRowNo;                               // 当前选择行
	QSharedPointer<ImageInstance> pImageIns;
	QSharedPointer<Marco> pMarco;

// 控件
private:
	QTableWidget *pMarcoTable;
	FastBtnArea *pFastBtnArea;
	MarcoPipeArea *pMarcoPipeArea;
	QGridLayout *pLayout;

// 槽函数
public slots:

	// 表格按钮单击槽函数
	void TblBtnClickSlot
	(
		enButtonType BType, 
		QString StrMarcoLabel
	);

	// 行选择更改槽函数
	void SelChangeSlot();

// 信号
signals:
	
	// 执行宏处理信号
	void ExcuteEmit(QString StrMarcoLabel);

	// 删除宏处理信号
	void DeleteEmit(QString StrMarcoLabel);

	// 快速按钮处理信号
	void FastBtnEmit(QString StrMarcoLabel);

// 函数
public:

	// 初始化
	void InitUI();

	// 宏表格更新
	void MarcoTableUpdate();

	// 宏处理管道显示更新
	void MarcoPipeUpdate();

	// 快速按钮显示更新
	void FastBtnUpdate();
};

