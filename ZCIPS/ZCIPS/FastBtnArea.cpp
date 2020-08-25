
#include "FastBtnArea.h"

/********************* FastButton START **********************/
/**************************************************************
函数名： FastButton

定义 ：  构造函数

入口参数 ：
	QWidget *parent ：	父指针

函数返回值 ：

****************************************************************/
FastButton::FastButton(QWidget *parent)
	: QPushButton(parent)
{
	// 槽函数与信号链接
	connect(this, SIGNAL(clicked(bool)), 
		this, SLOT(FastBtnClick(bool)));
}

/**************************************************************
函数名： FastBtnClick

定义 ：  单击事件槽函数

入口参数 ：
	bool bIsChecked : 

函数返回值 ：

****************************************************************/
void FastButton::FastBtnClick(bool bIsChecked)
{
	// 参数声明
	QString StrLabel;

	// 获取显示标签
	StrLabel = this->text();
	
	// 发送信号
	emit FastBtn(StrLabel);

	return;
}

/********************* FastButton END **********************/





/********************* FastBtnArea START **********************/

/**************************************************************
函数名： FastBtnArea

定义 ：  构造函数

入口参数 ：
	QWidget *parent ：					父指针

函数返回值 ：

****************************************************************/
FastBtnArea::FastBtnArea(QWidget *parent)
	: QScrollArea(parent)
{
	// 参数声明
	bool bIsBtn;			// 是否为快速处理按钮
	QWidget *pFastBtnAreaWidget;   

	// 设置Widget
	pFastBtnAreaWidget = new QWidget();
	pFastBtnAreaWidget->setObjectName(QStringLiteral("FastBtnAreaWidget"));
	this->setWidget(pFastBtnAreaWidget);
	pFastBtnAreaWidget->setContentsMargins(0, 0, 0, 0);

	// FastBtnArea设置
	this->setObjectName(QStringLiteral("FastBtnArea"));
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	this->setWidgetResizable(true);

	// Layout初始化
	pBtnLayout = new QGridLayout(this->widget());
	pBtnLayout->setContentsMargins(0, 0, 0, 0);
}

/**************************************************************
函数名： FastBtnUpdate

定义 ：  快速按钮区域更新

入口参数 ：
	QSharedPointer<Marco> pInMarco ：	宏处理类的指针

函数返回值 ：

****************************************************************/
void FastBtnArea::FastBtnUpdate(QSharedPointer<Marco> pInMarco)
{
	// 参数声明	
	int iBtnSize;
	int iMarcoSize;  		// 宏数量
	QString StrMarcoLabel;  // 宏处理管道标识
	FastButton *pFastBtn;   // 快速按钮控件指针

	// 清除快速按钮区域
	Clear();

	// 参数赋值
	iBtnSize = 0;
	iMarcoSize = pInMarco.get()->GetMarcoSize();

	// 按钮生成
	for (int i = 0; i < iMarcoSize; i++)
	{
		// 判断是否存在快速按钮
		// 存在，则加载至快速按钮区域
		if (true == pInMarco.get()->IsFastBtn(i))
		{
			// 添加快速按钮
			pFastBtn = new FastButton();

			// 设置按钮参数
			pFastBtn->setFixedWidth(FASTBTN_UNIT_WIDTH);
			pFastBtn->setFixedHeight(FASTBTN_UNIT_HEIGHT);
			StrMarcoLabel = pInMarco.get()->GetMarcoLabel(i);
			pFastBtn->setText(StrMarcoLabel);

			// 将按钮添加进Layout
			pBtnLayout->addWidget(pFastBtn, 0, iBtnSize);

			// 将按钮指针置入vec
			vecFastBtn.push_back(pFastBtn);

			// 按钮数量步进增长
			iBtnSize++;
		}
	}

	// 设置Widget宽度
	this->widget()->setFixedWidth(
		vecFastBtn.size() * FASTBTN_UNIT_WIDTH + FASTBTN_MARGIN);

	return;
}

/**************************************************************
函数名： Clear

定义 ：  清除快速按钮

入口参数 ：

函数返回值 ：

****************************************************************/
void FastBtnArea::Clear()
{
	// 参数声明
	int iFastBtnSize;
	FastButton *pFastBtn;
	// QTableWidget *pTableWidget;

	// 快速按钮长度获取
	iFastBtnSize = vecFastBtn.size();
	if (0 == iFastBtnSize) return;

	// 删除加载进网格的Widget
	while (0 != vecFastBtn.size())
	{
		pFastBtn = vecFastBtn[0];
		pBtnLayout->removeWidget(pFastBtn);
		vecFastBtn.pop_front();
		delete pFastBtn;
	}

	// 宽度设置
	this->widget()->setFixedWidth(0);

	return;
}

/**************************************************************
函数名： GetBtnVec

定义 ：  返回按钮控件Vec

入口参数 ：

函数返回值 ：

****************************************************************/
QVector<FastButton *> FastBtnArea::GetBtnVec()
{
	return vecFastBtn;
}

/********************* FastBtnArea END **********************/