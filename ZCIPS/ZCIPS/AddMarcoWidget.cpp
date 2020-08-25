#pragma once

#include "AddMarcoWidget.h"

/**************************************************************
函数名： AddMarcoWidget

定义 ：  构造函数

入口参数 ：
	QSharedPointer<Marco> pMarco ：				宏指针
	QSharedPointer<ImageInstance> pImageIns ：  图像实例指针
	QWidget *parent ：							父指针

函数返回值 ：

****************************************************************/
AddMarcoWidget::AddMarcoWidget
(
	QSharedPointer<Marco> pInMarco,
	QSharedPointer<ImageInstance> pInImageIns,
	QWidget *parent
)
	:QWidget(parent)
{
	// 指针赋值
	pMarco = pInMarco;
	pImageIns = pInImageIns;
	
	// 界面初始化
	InitUI();
	
	// 加载图像处理管道信息
	LoadProPipeInfo();

	// 消息与槽函数链接
	connect(pBtnAdd, SIGNAL(clicked()), this, SLOT(GenerateSlot()));
}

/**************************************************************
函数名： InitUI

定义 ：  界面初始化

入口参数 ：

函数返回值 ：

****************************************************************/
void AddMarcoWidget::InitUI()
{
	// 参数声明
	QString StrTitle;
	QString StrLabel;
	QString StrButton;
	QTextCodec* pTCode;		// 字符类型变换指针
	
	// 窗口名设置
	pTCode = QTextCodec::codecForName("GBK");
	StrTitle = pTCode->toUnicode("宏生成");
	StrLabel = pTCode->toUnicode("输入宏名：");
	StrButton = pTCode->toUnicode("生成");

	// 主窗口设置
	this->setFixedHeight(180);
	this->setFixedWidth(400);
	this->setWindowTitle(StrTitle);
	this->setWindowFlags(this->windowFlags()
		&~ Qt::WindowMinimizeButtonHint
		&~ Qt::WindowMinMaxButtonsHint);
	this->setAttribute(Qt::WA_ShowModal, true);

	// 初始化
	pLayout = new QGridLayout(this);
	pBtnAdd = new QToolButton;
	pPipArea = new MarcoPipeArea;
	pEdit = new QLineEdit;
	pLabel = new QLabel(StrLabel);

	// 参数设置
	pBtnAdd->setText(StrButton);
	pLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	pEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	pBtnAdd->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	// 布局设置
	pLayout->addWidget(pLabel, 0, 0, 1, 2);
	pLayout->addWidget(pEdit, 0, 2, 1, 6);
	pLayout->addWidget(pBtnAdd, 0, 8, 1, 2);
	pLayout->addWidget(pPipArea, 1, 0, 2, 10);
	
	// 设置该Grid为界面的Grid
	this->setLayout(pLayout);
}

/**************************************************************
函数名： LoadProPipeInfo

定义 ：  加载图像处理管道信息

入口参数 ：

函数返回值 ：

****************************************************************/
void AddMarcoWidget::LoadProPipeInfo()
{
	// 参数声明
	int iImgProcessSize;
	int iRet;
	QString StrPara;
	QString StrProcessName;
	ProcessType InType;
	ImageProcess *pImageProcess;
	GrayTransProcess *pGrayTransProcess;
	QTextCodec *pTCode;

	// 重置当前的宏处理管道显示
	pPipArea->ClearPipe();

	// 获取当前宏处理管道的长度
	iImgProcessSize = pImageIns.get()->pImageProcessIns.get()
		->pProcessPipe.get()->GetPipeLen();

	// 加载入宏处理通道
	for (int i = 0; i < iImgProcessSize; i++)
	{
		// 获取处理类型与参数
		iRet = pImageIns.get()->pImageProcessIns.get()
			->pProcessPipe.get()->PipeGet(i, InType, &pImageProcess);
		if (SUCCESS != iRet) return;

		// 获取参数字符
		switch (InType)
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
		pPipArea->AddPipeUnit(StrProcessName, StrPara);
	}

	// 选择
	pPipArea->SelectWidget();

	return;
}

/**************************************************************
函数名： GenerateSlot

定义 ：  界面初始化

入口参数 ：
	QPair<QString, QSharedPointer<ImageInstance>> PairImageIns

函数返回值 ：

****************************************************************/
void AddMarcoWidget::GenerateSlot()
{
	// 参数声明
	QString StrMarcoName;
	QTextCodec *pTCode;

	// 
	pTCode = QTextCodec::codecForName("GBK");

	// 获取宏名
	StrMarcoName = pEdit->text();

	// 判断宏名是否输入
	if (true == StrMarcoName.isEmpty())
	{
		QMessageBox::information(NULL,
			pTCode->toUnicode("提示"),
			pTCode->toUnicode("请输入宏名"),
			QMessageBox::Yes);
		
		return;
	}

	// 加载进处理宏
	pMarco.get()->InsertProPipe(StrMarcoName,
		pImageIns.get()->pImageProcessIns.get()->pProcessPipe);

	// 关闭函数
	this->close();

	return;
}