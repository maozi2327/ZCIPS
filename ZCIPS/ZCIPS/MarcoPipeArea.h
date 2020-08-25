#pragma once

#include <QtWidgets>

/***************************** 宏 ******************************/
#define MARCOPIPE_NO_SELECT			  -1    // 当前无选择
#define MARCOPIPLE_UNIT_WIDTH         100   // 宏处理单元显示宽度


/***************************** 类 ******************************/
class MarcoPipeArea : public QScrollArea
{
	Q_OBJECT

// 参数
private:
	int iSelNo;								// 当前选中的Widget编号
	QVector<QTableWidget *> vecPipeWidget;  // 内部的Widget列表
	QGridLayout *pPipeLayout;               // 处理管道Layout

// 响应与重载
public:

	// 初始化
	MarcoPipeArea(QWidget *parent = nullptr);

// 槽函数
public slots:

	// 鼠标点击槽函数
	void MousePressSlot(int iRow, int iCollum);

// 函数
public:

	// 添加处理管道显示内容
	void AddPipeUnit
	(
		QString StrProcessName,
		QString StrProcessContent
	);

	// 清除整个处理管道
	void ClearPipe();

	// 获取当前选择编号
	int GetCurrSel();

	// 从当选择位置截断
	void CutFromCurrSel();

	// 选中当前得Widget
	void SelectWidget();
};