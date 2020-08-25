#include "MarcoPipeArea.h"

/**************************************************************
函数名： MarcoPipeArea

定义 ：  构造函数

入口参数 ：
	QWidget *parent ： 父指针

函数返回值 ：

****************************************************************/
MarcoPipeArea::MarcoPipeArea(QWidget* parent)
	: QScrollArea(parent)
{
	// 参数声明
	QWidget *pMarcoPipeAreaWidget;
	
	// 参数初始化
	iSelNo = MARCOPIPE_NO_SELECT;

	// 设置Widget
	pMarcoPipeAreaWidget = new QWidget();
	pMarcoPipeAreaWidget->setObjectName(QStringLiteral("MarcoPipeAreaWidget"));
	this->setWidget(pMarcoPipeAreaWidget);

	// MarcoPipeArea设置
	this->setObjectName(QStringLiteral("MarcoPipeArea"));
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	this->setWidgetResizable(true);

	// 控件初始化
	pPipeLayout = new QGridLayout(this->widget());
}

/**************************************************************
函数名： MousePressSlot

定义 ：  鼠标点击事件槽函数

入口参数 ：
	int iRow ：    行值
	int iCollum ： 列值

函数返回值 ：

****************************************************************/
void MarcoPipeArea::MousePressSlot(int iRow, int iCollum)
{
	// 遍历所有Widget
	for (int i = 0; i < vecPipeWidget.size(); i++)
	{
		if (vecPipeWidget[i]->underMouse())
		{			
			// 当前选择编号赋值
			iSelNo = i;
	
			break;
		}
	}

	// 边缘绘制
	for (int i = 0; i < vecPipeWidget.size(); i++)
	{
		if (i == iSelNo) 
			vecPipeWidget[i]->setStyleSheet("background-color:white;border: 2px solid #33CCFF;");
		else
			vecPipeWidget[i]->setStyleSheet("background-color:white;border: 1px solid #C4C4C3;");
	}
}

/**************************************************************
函数名： AddPipeUnit

定义 ：  添加处理管道显示内容

入口参数 ：
	QString StrProcessName  ：		处理名
	QString StrProcessContent  ：	处理内容

函数返回值 ：

****************************************************************/
void MarcoPipeArea::AddPipeUnit
(
	QString StrProcessName,
	QString StrProcessContent
)
{
	// 参数声明
	int iLastUnitPos;
	QTableWidget *pTblWidget;
	QTableWidgetItem *pTableItem;

	// 初始化
	pTblWidget = new QTableWidget();

	// 表格参数设置
	pTblWidget->setRowCount(2);
	pTblWidget->setColumnCount(1);
	pTblWidget->setColumnWidth(0, MARCOPIPLE_UNIT_WIDTH);
	pTblWidget->verticalHeader()->setVisible(false);
	pTblWidget->horizontalHeader()->setVisible(false);
	pTblWidget->setWordWrap(true);
	pTblWidget->setStyleSheet("background-color:white;border: 1px solid #C4C4C3;");

	// 表格内容设置
	pTableItem = new QTableWidgetItem;
	pTableItem->setText(StrProcessName);
	pTblWidget->setItem(0, 0, pTableItem);
	pTableItem = new QTableWidgetItem;
	pTableItem->setText(StrProcessContent);
	pTblWidget->setItem(1, 0, pTableItem);
	pTblWidget->resizeRowsToContents();
	pTblWidget->setMouseTracking(true);
	
	// 添加链接
	connect(pTblWidget, SIGNAL(cellClicked(int, int)), this, SLOT(MousePressSlot(int, int)));

	// 将QWidget加入队列
	vecPipeWidget.push_back(pTblWidget);

	// 将表格加载入网格
	iLastUnitPos = vecPipeWidget.size() - 1;
	pPipeLayout->addWidget(pTblWidget, 0, iLastUnitPos);

	// 宽度设置
	this->widget()->setFixedWidth(vecPipeWidget.size() * MARCOPIPLE_UNIT_WIDTH);

	return;
}

/**************************************************************
函数名： ClearPipe

定义 ：  清除整个处理管道

入口参数 ：

函数返回值 ：

****************************************************************/
void MarcoPipeArea::ClearPipe()
{
	// 参数声明
	int iPipleSize;
	QTableWidget *pTableWidget;

	// 处理管道长度获取
	iPipleSize = vecPipeWidget.size();
	if (0 == iPipleSize) return;

	// 删除加载进网格的Widget
	while (0 != vecPipeWidget.size())
	{
		pTableWidget = vecPipeWidget[0];
		pPipeLayout->removeWidget(pTableWidget);
		vecPipeWidget.pop_front();
		delete pTableWidget;
	}

	// 宽度设置
	this->widget()->setFixedWidth(0);

	return;
}

/**************************************************************
函数名： GetCurrSel

定义 ：  获取当前选择编号

入口参数 ：

函数返回值 ：
	int iSelNo : 当前选择编号
****************************************************************/
int MarcoPipeArea::GetCurrSel()
{
	return iSelNo;
}

/**************************************************************
函数名： CutFromCurrSel

定义 ：  从当选择位置截断

入口参数 ：

函数返回值 ：
	
****************************************************************/
void MarcoPipeArea::CutFromCurrSel()
{
	// 参数声明
	QTableWidget *pTblWidget;
	
	// 当前选择编号检查
	if ((iSelNo == MARCOPIPE_NO_SELECT)
		|| (iSelNo >= vecPipeWidget.size()))
	{
		return;
	}

	// 删除对应得Widget
	while (vecPipeWidget.size() > iSelNo)
	{
		pTblWidget = vecPipeWidget[iSelNo];
		pPipeLayout->removeWidget(pTblWidget);
		vecPipeWidget.removeAt(iSelNo);
		delete pTblWidget;
	}

	// 设置控件宽度
	this->widget()->setFixedWidth(vecPipeWidget.size() * MARCOPIPLE_UNIT_WIDTH);

	// 选择编号赋值
	iSelNo = MARCOPIPE_NO_SELECT;
}

/**************************************************************
函数名： SelectWidget

定义 ：  选中当前得Widget

入口参数 ：

函数返回值 ：

****************************************************************/
void MarcoPipeArea::SelectWidget()
{
	// 当前选择编号检查
	if ((iSelNo == MARCOPIPE_NO_SELECT)
		|| (iSelNo >= vecPipeWidget.size()))
	{
		return;
	}

	// 边缘绘制
	for (int i = 0; i < vecPipeWidget.size(); i++)
	{
		if (i == iSelNo)
			vecPipeWidget[i]->setStyleSheet("background-color:white;border: 2px solid #33CCFF;");
		else
			vecPipeWidget[i]->setStyleSheet("background-color:white;border: 1px solid #C4C4C3;");
	}
}