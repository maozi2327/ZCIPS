#pragma once

#include "AddMarcoWidget.h"

/**************************************************************
�������� AddMarcoWidget

���� ��  ���캯��

��ڲ��� ��
	QSharedPointer<Marco> pMarco ��				��ָ��
	QSharedPointer<ImageInstance> pImageIns ��  ͼ��ʵ��ָ��
	QWidget *parent ��							��ָ��

��������ֵ ��

****************************************************************/
AddMarcoWidget::AddMarcoWidget
(
	QSharedPointer<Marco> pInMarco,
	QSharedPointer<ImageInstance> pInImageIns,
	QWidget *parent
)
	:QWidget(parent)
{
	// ָ�븳ֵ
	pMarco = pInMarco;
	pImageIns = pInImageIns;
	
	// �����ʼ��
	InitUI();
	
	// ����ͼ����ܵ���Ϣ
	LoadProPipeInfo();

	// ��Ϣ��ۺ�������
	connect(pBtnAdd, SIGNAL(clicked()), this, SLOT(GenerateSlot()));
}

/**************************************************************
�������� InitUI

���� ��  �����ʼ��

��ڲ��� ��

��������ֵ ��

****************************************************************/
void AddMarcoWidget::InitUI()
{
	// ��������
	QString StrTitle;
	QString StrLabel;
	QString StrButton;
	QTextCodec* pTCode;		// �ַ����ͱ任ָ��
	
	// ����������
	pTCode = QTextCodec::codecForName("GBK");
	StrTitle = pTCode->toUnicode("������");
	StrLabel = pTCode->toUnicode("���������");
	StrButton = pTCode->toUnicode("����");

	// ����������
	this->setFixedHeight(180);
	this->setFixedWidth(400);
	this->setWindowTitle(StrTitle);
	this->setWindowFlags(this->windowFlags()
		&~ Qt::WindowMinimizeButtonHint
		&~ Qt::WindowMinMaxButtonsHint);
	this->setAttribute(Qt::WA_ShowModal, true);

	// ��ʼ��
	pLayout = new QGridLayout(this);
	pBtnAdd = new QToolButton;
	pPipArea = new MarcoPipeArea;
	pEdit = new QLineEdit;
	pLabel = new QLabel(StrLabel);

	// ��������
	pBtnAdd->setText(StrButton);
	pLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	pEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	pBtnAdd->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	// ��������
	pLayout->addWidget(pLabel, 0, 0, 1, 2);
	pLayout->addWidget(pEdit, 0, 2, 1, 6);
	pLayout->addWidget(pBtnAdd, 0, 8, 1, 2);
	pLayout->addWidget(pPipArea, 1, 0, 2, 10);
	
	// ���ø�GridΪ�����Grid
	this->setLayout(pLayout);
}

/**************************************************************
�������� LoadProPipeInfo

���� ��  ����ͼ����ܵ���Ϣ

��ڲ��� ��

��������ֵ ��

****************************************************************/
void AddMarcoWidget::LoadProPipeInfo()
{
	// ��������
	int iImgProcessSize;
	int iRet;
	QString StrPara;
	QString StrProcessName;
	ProcessType InType;
	ImageProcess *pImageProcess;
	GrayTransProcess *pGrayTransProcess;
	QTextCodec *pTCode;

	// ���õ�ǰ�ĺ괦��ܵ���ʾ
	pPipArea->ClearPipe();

	// ��ȡ��ǰ�괦��ܵ��ĳ���
	iImgProcessSize = pImageIns.get()->pImageProcessIns.get()
		->pProcessPipe.get()->GetPipeLen();

	// ������괦��ͨ��
	for (int i = 0; i < iImgProcessSize; i++)
	{
		// ��ȡ�������������
		iRet = pImageIns.get()->pImageProcessIns.get()
			->pProcessPipe.get()->PipeGet(i, InType, &pImageProcess);
		if (SUCCESS != iRet) return;

		// ��ȡ�����ַ�
		switch (InType)
		{
			case ProcessType_GrayTrans:
			{
				// ��ȡ����
				iRet = pImageProcess->GetParaString(StrPara);
				if (SUCCESS != iRet) return;

				// ��������
				pTCode = QTextCodec::codecForName("GBK");
				StrProcessName = pTCode->toUnicode("�Ҷȱ任");

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

		// ���ؽ���ScrollArea�ؼ�
		pPipArea->AddPipeUnit(StrProcessName, StrPara);
	}

	// ѡ��
	pPipArea->SelectWidget();

	return;
}

/**************************************************************
�������� GenerateSlot

���� ��  �����ʼ��

��ڲ��� ��
	QPair<QString, QSharedPointer<ImageInstance>> PairImageIns

��������ֵ ��

****************************************************************/
void AddMarcoWidget::GenerateSlot()
{
	// ��������
	QString StrMarcoName;
	QTextCodec *pTCode;

	// 
	pTCode = QTextCodec::codecForName("GBK");

	// ��ȡ����
	StrMarcoName = pEdit->text();

	// �жϺ����Ƿ�����
	if (true == StrMarcoName.isEmpty())
	{
		QMessageBox::information(NULL,
			pTCode->toUnicode("��ʾ"),
			pTCode->toUnicode("���������"),
			QMessageBox::Yes);
		
		return;
	}

	// ���ؽ������
	pMarco.get()->InsertProPipe(StrMarcoName,
		pImageIns.get()->pImageProcessIns.get()->pProcessPipe);

	// �رպ���
	this->close();

	return;
}