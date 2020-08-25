
#include "LoadMarcoWidget.h"
#include "ERRInfo.h"

/********************* LoadMarcoWidget START ************************/
/*****************************************************
函数名： LoadMarcoWidget

定义 ：  构造函数

入口参数 ：
	enButtonType BInType ：
	QString StrInMarcoLabel ：
	QWidget *parent ：				父指针

函数返回值 ：

******************************************************/
TableButton::TableButton
(
	enButtonType BInType,
	QString StrInMarcoLabel, 
	QWidget *parent
)
	: QPushButton(parent)
{
	// 参数声明
	QTextCodec* pTCode;		// 字符类型变换指针

	// 设置汉字编码
	pTCode = QTextCodec::codecForName("GBK");

	// 参数赋值
	m_BType = BInType;
	StrMarcoLabel = StrInMarcoLabel;

	// 设置按钮名
	switch (m_BType)
	{
		case ButtonType_Excute:
		{
			this->setText(pTCode->toUnicode("执行"));
			break;
		}
		case ButtonType_Delete:
		{
			this->setText(pTCode->toUnicode("删除"));
			break;
		}
		case ButtonType_FastBtn:
		{
			this->setText(pTCode->toUnicode("快速按钮"));
			break;
		}
		default: break;
	}

	// 槽函数与消息链接
	connect(this, SIGNAL(clicked(bool)),
		this, SLOT(ClickSlot(bool)));
}

/*****************************************************
函数名： ClickSlot

定义 ：  点击槽函数

入口参数 ：
	bool bChecked ：	指针

函数返回值 ：

******************************************************/
void TableButton::ClickSlot(bool bChecked)
{
	emit ClickEmit(m_BType, StrMarcoLabel);
}
/********************* LoadMarcoWidget END **************************/



/********************* LoadMarcoWidget START **************************/
/*****************************************************
函数名： LoadMarcoWidget

定义 ：  构造函数

入口参数 ：
	QSharedPointer<ImageInstance> pInImageIns ：图像实例指针
	QSharedPointer<Marco> pInMarco ：			宏处理指针
	QWidget *parent ：							父指针

函数返回值 ：

******************************************************/
LoadMarcoWidget::LoadMarcoWidget
(
	QSharedPointer<ImageInstance> pInImageIns,
	QSharedPointer<Marco> pInMarco,
	QWidget *parent
)
	: QWidget(parent)
{
	// 赋值
	iSelRowNo = NO_SELECTION;
	
	// 指针赋值
	pImageIns = pInImageIns;
	pMarco = pInMarco;

	// 界面初始化
	InitUI();

	// 信号与槽函数链接
	connect(pMarcoTable, SIGNAL(itemSelectionChanged()),
		this, SLOT(SelChangeSlot()));

}

/**************************************************************
函数名： InitUI

定义 ：  界面初始化

入口参数 ：

函数返回值 ：

****************************************************************/
void LoadMarcoWidget::InitUI()
{
	// 参数声明
	QString StrTitle;
	QString StrLabel;
	QStringList StrTblHeader;   
	QHeaderView *pHeaderView;
	QTextCodec* pTCode;			// 字符类型变换指针
	
	// 窗口名设置
	pTCode = QTextCodec::codecForName("GBK");
	StrTitle = pTCode->toUnicode("宏加载/编辑");

	// 主窗口设置
	this->setFixedHeight(550);
	this->setFixedWidth(500);
	this->setWindowTitle(StrTitle);
	this->setWindowFlags(this->windowFlags()
		&~Qt::WindowMinimizeButtonHint
		&~Qt::WindowMinMaxButtonsHint);
	this->setAttribute(Qt::WA_ShowModal, true);
	
	// 初始化
	pMarcoTable = new QTableWidget;
	pLayout = new QGridLayout(this);
	pMarcoPipeArea = new MarcoPipeArea;
	pFastBtnArea = new FastBtnArea;

	// 布局设置
	pLayout->addWidget(pMarcoTable, 0, 0, 12, 1);
	pLayout->addWidget(pMarcoPipeArea, 12, 0, 5, 1);
	pLayout->addWidget(pFastBtnArea, 17, 0, 3, 1);

	// 设置该Grid为界面的Grid
	this->setLayout(pLayout);

	// TableWidgets控件设置
	pMarcoTable->setColumnCount(4);
	StrTblHeader.append(pTCode->toUnicode("ID"));
	StrTblHeader.append(pTCode->toUnicode("宏处理名"));
	StrTblHeader.append(pTCode->toUnicode("快速按钮"));
	StrTblHeader.append("");
	pMarcoTable->setHorizontalHeaderLabels(StrTblHeader);
	pMarcoTable->setColumnWidth(0, 30);
	pMarcoTable->setColumnWidth(1, 170);
	pMarcoTable->setColumnWidth(2, 70);
	pMarcoTable->setColumnWidth(3, 205);
	pHeaderView = pMarcoTable->verticalHeader();
	pHeaderView->setHidden(true);
	pHeaderView = pMarcoTable->horizontalHeader();
	pHeaderView->setStyleSheet("border: 1px groove gray;");
	pMarcoTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	pMarcoTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	pMarcoTable->setSelectionMode(QAbstractItemView::SingleSelection);

	// 宏处理表格更新
	MarcoTableUpdate();
	
	// 处理管道更新
	MarcoPipeUpdate();

	// 快速处理按钮区域更新 
	FastBtnUpdate();

	return;
}


/**************************************************************
函数名： MarcoTableUpdate

定义 ：  宏表格更新

入口参数 ：

函数返回值 ：

****************************************************************/
void LoadMarcoWidget::MarcoTableUpdate()
{
	// 参数声明
	int iRows;
	int iMarcoSize;
	QWidget *pBtnWidget;
	QGridLayout *pLayout;
	QTableWidgetItem *pItem;
	TableButton *pTblButton;
	
	// 清除表格
	iRows = pMarcoTable->rowCount();
	for (int iRowNo = 0; iRowNo < iRows; iRowNo++)
		pMarcoTable->removeRow(iRowNo);

	// 参数赋值
	iMarcoSize = pMarco.get()->GetMarcoSize();
	pMarcoTable->setRowCount(iMarcoSize);

	// 写入宏信息
	for (int iItemNo = 0; iItemNo < iMarcoSize; iItemNo++)
	{
		// 加载ID编号
		pItem = new QTableWidgetItem(QString::number(iItemNo + 1));
		pMarcoTable->setItem(iItemNo, 0, pItem);
		pMarcoTable->item(iItemNo, 0)->setTextAlignment(Qt::AlignCenter);

		// 加载宏名
		pItem = new QTableWidgetItem(pMarco.get()->GetMarcoLabel(iItemNo));
		pMarcoTable->setItem(iItemNo, 1, pItem);

		// 判断是否具有快速按钮
		if (true == pMarco.get()->IsFastBtn(iItemNo))
		{
			pItem = new QTableWidgetItem("Yes" );
			pMarcoTable->setItem(iItemNo, 2, pItem);
			pMarcoTable->item(iItemNo, 2)
				->setTextAlignment(Qt::AlignCenter);
		}
		else
		{
			pItem = new QTableWidgetItem("");
			pMarcoTable->setItem(iItemNo, 2, pItem);
			pMarcoTable->item(iItemNo, 2)
				->setTextAlignment(Qt::AlignCenter);
		}

		// 加载按钮界面
		pLayout = new QGridLayout();
		pBtnWidget = new QWidget();
		pBtnWidget->setLayout(pLayout);
		pLayout->setContentsMargins(0, 0, 0, 0);
		pMarcoTable->setCellWidget(iItemNo, 3, pBtnWidget);

		// 加载执行按钮
		pTblButton = new TableButton(
			ButtonType_Excute, 
			pMarco.get()->GetMarcoLabel(iItemNo),
			nullptr);
		pTblButton->setFixedWidth(60);
		connect(pTblButton, SIGNAL(ClickEmit(enButtonType, QString)),
			this, SLOT(TblBtnClickSlot(enButtonType, QString)));
		if (true == pImageIns.isNull()) pTblButton->setEnabled(false);
		pLayout->addWidget(pTblButton, 0, 0);
		
		// 加载删除按钮
		pTblButton = new TableButton(
			ButtonType_Delete,
			pMarco.get()->GetMarcoLabel(iItemNo),
			nullptr);
		pTblButton->setFixedWidth(60);
		connect(pTblButton, SIGNAL(ClickEmit(enButtonType, QString)),
			this, SLOT(TblBtnClickSlot(enButtonType, QString)));
		pLayout->addWidget(pTblButton, 0, 1);

		// 加载快速按钮按钮
		pTblButton = new TableButton(
			ButtonType_FastBtn,
			pMarco.get()->GetMarcoLabel(iItemNo),
			nullptr);
		pTblButton->setFixedWidth(60);
		connect(pTblButton, SIGNAL(ClickEmit(enButtonType, QString)),
			this, SLOT(TblBtnClickSlot(enButtonType, QString)));
		pLayout->addWidget(pTblButton, 0, 2);
	}

	return;
}

/**************************************************************
函数名： MarcoPipeUpdate

定义 ：  宏处理管道显示更新

入口参数 ：

函数返回值 ：

****************************************************************/
void LoadMarcoWidget::MarcoPipeUpdate()
{
	// 参数声明
	int iRet;
	int iPipeSize;
	QString StrMarcoLabel;
	QString StrPara;
	QString StrProcessName;
	ProcessType m_PType;
	QSharedPointer<ImageProPipe> pProPipe;
	QTextCodec *pTCode;
	ImageProcess *pImageProcess;

	// 重置当前的宏处理管道显示
	pMarcoPipeArea->ClearPipe();

	// 当前选择编号判断
	if (NO_SELECTION == iSelRowNo) return;

	// 获取当前处理宏的标识
	StrMarcoLabel = pMarco.get()->GetMarcoLabel(iSelRowNo);

	// 获取图像处理管道
	pProPipe = pMarco.get()->GetImageProPipe(StrMarcoLabel);
	if (true == pProPipe.isNull()) return;
	
	// 获取处理管道尺寸
	iPipeSize = pProPipe.get()->GetPipeLen();
	if (0 == iPipeSize) return;

	// 加载入宏处理通道
	for (int iProNo = 0; iProNo < iPipeSize; iProNo++)
	{
		// 获取处理类型与参数
		iRet = pProPipe.get()->PipeGet
			(iProNo, m_PType, &pImageProcess);
		if (SUCCESS != iRet) return;

		// 获取参数字符
		switch (m_PType)
		{
			case ProcessType_GrayTrans:
			{
				// 获取参数
				iRet = pImageProcess->GetParaString(StrPara);
				if (SUCCESS != iRet) return;

				// 处理步骤名
				pTCode = QTextCodec::codecForName("GBK");
				StrProcessName = pTCode->toUnicode("灰度变换");

				break;
			}

			case ProcessType_Enhancement:
			{
				continue;
			}

			case ProcessType_None:
			default:
			{
				continue;
			}
		}

		// 加载进入ScrollArea控件
		pMarcoPipeArea->AddPipeUnit(StrProcessName, StrPara);
	}

	// 选择
	pMarcoPipeArea->SelectWidget();
}

/**************************************************************
函数名： FastBtnUpdate

定义 ：  快速按钮显示更新

入口参数 ：

函数返回值 ：

****************************************************************/
void LoadMarcoWidget::FastBtnUpdate()
{
	// 参数声明
	QVector<FastButton *>::iterator it;
	QVector<FastButton *> vecFastBtn;

	// 更新区域选择
	pFastBtnArea->FastBtnUpdate(pMarco);

	return;
}

/**************************************************************
函数名： TblBtnClickSlot

定义 ：  快速按钮显示更新

入口参数 ：
	enButtonType BType ：		
	QString StrMarcoLabel ：	

函数返回值 ：

****************************************************************/
void LoadMarcoWidget::TblBtnClickSlot
(
	enButtonType BType,
	QString StrMarcoLabel
)
{
	// 按钮类型
	switch (BType)
	{
		// 宏处理执行
		case ButtonType_Excute:
		{
		    // 图像执行消息
			emit ExcuteEmit(StrMarcoLabel);
			
			// 关闭窗口
			this->close();

			break;
		}
		
		// 宏处理删除
		case ButtonType_Delete:
		{
			// 宏处理删除消息
			emit DeleteEmit(StrMarcoLabel);

			// 宏处理表格更新
			MarcoTableUpdate();

			// 选择编号赋值
			iSelRowNo = NO_SELECTION;

			// 宏处理管道显示区域更新
			MarcoPipeUpdate();

			// 快速按钮更新
			FastBtnUpdate();

			break;
		}

		// 快速处理按钮
		case ButtonType_FastBtn:
		{
			// 快速按钮信号
			emit FastBtnEmit(StrMarcoLabel);

			// 宏处理表格更新
			MarcoTableUpdate();

			// 选中行
			pMarcoTable->selectRow(iSelRowNo);

			// 快速按钮更新
			FastBtnUpdate();

			break;
		}

		default: break;
		
	}
	
	return;
}

/**************************************************************
函数名： SelChangeSlot

定义 ：  选择行改变

入口参数 ：

函数返回值 ：

****************************************************************/
void LoadMarcoWidget::SelChangeSlot()
{
	// 参数声明
	int iRowNo;

	// 行值获取
	iRowNo = pMarcoTable->selectedItems()[0]->row();

	// 行值判断
	if ((iSelRowNo == iRowNo) 
		|| (0 > iRowNo)
		|| (pMarco.get()->GetMarcoSize() <= iRowNo))
	{
		return;
	}

	// 选择行赋值
	iSelRowNo = iRowNo;

	// 变更MarcoPipeArea显示
	MarcoPipeUpdate();
	
	return;
}

/********************* LoadMarcoWidget END **************************/
