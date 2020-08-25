
#include "LoadMarcoWidget.h"
#include "ERRInfo.h"

/********************* LoadMarcoWidget START ************************/
/*****************************************************
�������� LoadMarcoWidget

���� ��  ���캯��

��ڲ��� ��
	enButtonType BInType ��
	QString StrInMarcoLabel ��
	QWidget *parent ��				��ָ��

��������ֵ ��

******************************************************/
TableButton::TableButton
(
	enButtonType BInType,
	QString StrInMarcoLabel, 
	QWidget *parent
)
	: QPushButton(parent)
{
	// ��������
	QTextCodec* pTCode;		// �ַ����ͱ任ָ��

	// ���ú��ֱ���
	pTCode = QTextCodec::codecForName("GBK");

	// ������ֵ
	m_BType = BInType;
	StrMarcoLabel = StrInMarcoLabel;

	// ���ð�ť��
	switch (m_BType)
	{
		case ButtonType_Excute:
		{
			this->setText(pTCode->toUnicode("ִ��"));
			break;
		}
		case ButtonType_Delete:
		{
			this->setText(pTCode->toUnicode("ɾ��"));
			break;
		}
		case ButtonType_FastBtn:
		{
			this->setText(pTCode->toUnicode("���ٰ�ť"));
			break;
		}
		default: break;
	}

	// �ۺ�������Ϣ����
	connect(this, SIGNAL(clicked(bool)),
		this, SLOT(ClickSlot(bool)));
}

/*****************************************************
�������� ClickSlot

���� ��  ����ۺ���

��ڲ��� ��
	bool bChecked ��	ָ��

��������ֵ ��

******************************************************/
void TableButton::ClickSlot(bool bChecked)
{
	emit ClickEmit(m_BType, StrMarcoLabel);
}
/********************* LoadMarcoWidget END **************************/



/********************* LoadMarcoWidget START **************************/
/*****************************************************
�������� LoadMarcoWidget

���� ��  ���캯��

��ڲ��� ��
	QSharedPointer<ImageInstance> pInImageIns ��ͼ��ʵ��ָ��
	QSharedPointer<Marco> pInMarco ��			�괦��ָ��
	QWidget *parent ��							��ָ��

��������ֵ ��

******************************************************/
LoadMarcoWidget::LoadMarcoWidget
(
	QSharedPointer<ImageInstance> pInImageIns,
	QSharedPointer<Marco> pInMarco,
	QWidget *parent
)
	: QWidget(parent)
{
	// ��ֵ
	iSelRowNo = NO_SELECTION;
	
	// ָ�븳ֵ
	pImageIns = pInImageIns;
	pMarco = pInMarco;

	// �����ʼ��
	InitUI();

	// �ź���ۺ�������
	connect(pMarcoTable, SIGNAL(itemSelectionChanged()),
		this, SLOT(SelChangeSlot()));

}

/**************************************************************
�������� InitUI

���� ��  �����ʼ��

��ڲ��� ��

��������ֵ ��

****************************************************************/
void LoadMarcoWidget::InitUI()
{
	// ��������
	QString StrTitle;
	QString StrLabel;
	QStringList StrTblHeader;   
	QHeaderView *pHeaderView;
	QTextCodec* pTCode;			// �ַ����ͱ任ָ��
	
	// ����������
	pTCode = QTextCodec::codecForName("GBK");
	StrTitle = pTCode->toUnicode("�����/�༭");

	// ����������
	this->setFixedHeight(550);
	this->setFixedWidth(500);
	this->setWindowTitle(StrTitle);
	this->setWindowFlags(this->windowFlags()
		&~Qt::WindowMinimizeButtonHint
		&~Qt::WindowMinMaxButtonsHint);
	this->setAttribute(Qt::WA_ShowModal, true);
	
	// ��ʼ��
	pMarcoTable = new QTableWidget;
	pLayout = new QGridLayout(this);
	pMarcoPipeArea = new MarcoPipeArea;
	pFastBtnArea = new FastBtnArea;

	// ��������
	pLayout->addWidget(pMarcoTable, 0, 0, 12, 1);
	pLayout->addWidget(pMarcoPipeArea, 12, 0, 5, 1);
	pLayout->addWidget(pFastBtnArea, 17, 0, 3, 1);

	// ���ø�GridΪ�����Grid
	this->setLayout(pLayout);

	// TableWidgets�ؼ�����
	pMarcoTable->setColumnCount(4);
	StrTblHeader.append(pTCode->toUnicode("ID"));
	StrTblHeader.append(pTCode->toUnicode("�괦����"));
	StrTblHeader.append(pTCode->toUnicode("���ٰ�ť"));
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

	// �괦�������
	MarcoTableUpdate();
	
	// ����ܵ�����
	MarcoPipeUpdate();

	// ���ٴ���ť������� 
	FastBtnUpdate();

	return;
}


/**************************************************************
�������� MarcoTableUpdate

���� ��  �������

��ڲ��� ��

��������ֵ ��

****************************************************************/
void LoadMarcoWidget::MarcoTableUpdate()
{
	// ��������
	int iRows;
	int iMarcoSize;
	QWidget *pBtnWidget;
	QGridLayout *pLayout;
	QTableWidgetItem *pItem;
	TableButton *pTblButton;
	
	// ������
	iRows = pMarcoTable->rowCount();
	for (int iRowNo = 0; iRowNo < iRows; iRowNo++)
		pMarcoTable->removeRow(iRowNo);

	// ������ֵ
	iMarcoSize = pMarco.get()->GetMarcoSize();
	pMarcoTable->setRowCount(iMarcoSize);

	// д�����Ϣ
	for (int iItemNo = 0; iItemNo < iMarcoSize; iItemNo++)
	{
		// ����ID���
		pItem = new QTableWidgetItem(QString::number(iItemNo + 1));
		pMarcoTable->setItem(iItemNo, 0, pItem);
		pMarcoTable->item(iItemNo, 0)->setTextAlignment(Qt::AlignCenter);

		// ���غ���
		pItem = new QTableWidgetItem(pMarco.get()->GetMarcoLabel(iItemNo));
		pMarcoTable->setItem(iItemNo, 1, pItem);

		// �ж��Ƿ���п��ٰ�ť
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

		// ���ذ�ť����
		pLayout = new QGridLayout();
		pBtnWidget = new QWidget();
		pBtnWidget->setLayout(pLayout);
		pLayout->setContentsMargins(0, 0, 0, 0);
		pMarcoTable->setCellWidget(iItemNo, 3, pBtnWidget);

		// ����ִ�а�ť
		pTblButton = new TableButton(
			ButtonType_Excute, 
			pMarco.get()->GetMarcoLabel(iItemNo),
			nullptr);
		pTblButton->setFixedWidth(60);
		connect(pTblButton, SIGNAL(ClickEmit(enButtonType, QString)),
			this, SLOT(TblBtnClickSlot(enButtonType, QString)));
		if (true == pImageIns.isNull()) pTblButton->setEnabled(false);
		pLayout->addWidget(pTblButton, 0, 0);
		
		// ����ɾ����ť
		pTblButton = new TableButton(
			ButtonType_Delete,
			pMarco.get()->GetMarcoLabel(iItemNo),
			nullptr);
		pTblButton->setFixedWidth(60);
		connect(pTblButton, SIGNAL(ClickEmit(enButtonType, QString)),
			this, SLOT(TblBtnClickSlot(enButtonType, QString)));
		pLayout->addWidget(pTblButton, 0, 1);

		// ���ؿ��ٰ�ť��ť
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
�������� MarcoPipeUpdate

���� ��  �괦��ܵ���ʾ����

��ڲ��� ��

��������ֵ ��

****************************************************************/
void LoadMarcoWidget::MarcoPipeUpdate()
{
	// ��������
	int iRet;
	int iPipeSize;
	QString StrMarcoLabel;
	QString StrPara;
	QString StrProcessName;
	ProcessType m_PType;
	QSharedPointer<ImageProPipe> pProPipe;
	QTextCodec *pTCode;
	ImageProcess *pImageProcess;

	// ���õ�ǰ�ĺ괦��ܵ���ʾ
	pMarcoPipeArea->ClearPipe();

	// ��ǰѡ�����ж�
	if (NO_SELECTION == iSelRowNo) return;

	// ��ȡ��ǰ�����ı�ʶ
	StrMarcoLabel = pMarco.get()->GetMarcoLabel(iSelRowNo);

	// ��ȡͼ����ܵ�
	pProPipe = pMarco.get()->GetImageProPipe(StrMarcoLabel);
	if (true == pProPipe.isNull()) return;
	
	// ��ȡ����ܵ��ߴ�
	iPipeSize = pProPipe.get()->GetPipeLen();
	if (0 == iPipeSize) return;

	// ������괦��ͨ��
	for (int iProNo = 0; iProNo < iPipeSize; iProNo++)
	{
		// ��ȡ�������������
		iRet = pProPipe.get()->PipeGet
			(iProNo, m_PType, &pImageProcess);
		if (SUCCESS != iRet) return;

		// ��ȡ�����ַ�
		switch (m_PType)
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
		pMarcoPipeArea->AddPipeUnit(StrProcessName, StrPara);
	}

	// ѡ��
	pMarcoPipeArea->SelectWidget();
}

/**************************************************************
�������� FastBtnUpdate

���� ��  ���ٰ�ť��ʾ����

��ڲ��� ��

��������ֵ ��

****************************************************************/
void LoadMarcoWidget::FastBtnUpdate()
{
	// ��������
	QVector<FastButton *>::iterator it;
	QVector<FastButton *> vecFastBtn;

	// ��������ѡ��
	pFastBtnArea->FastBtnUpdate(pMarco);

	return;
}

/**************************************************************
�������� TblBtnClickSlot

���� ��  ���ٰ�ť��ʾ����

��ڲ��� ��
	enButtonType BType ��		
	QString StrMarcoLabel ��	

��������ֵ ��

****************************************************************/
void LoadMarcoWidget::TblBtnClickSlot
(
	enButtonType BType,
	QString StrMarcoLabel
)
{
	// ��ť����
	switch (BType)
	{
		// �괦��ִ��
		case ButtonType_Excute:
		{
		    // ͼ��ִ����Ϣ
			emit ExcuteEmit(StrMarcoLabel);
			
			// �رմ���
			this->close();

			break;
		}
		
		// �괦��ɾ��
		case ButtonType_Delete:
		{
			// �괦��ɾ����Ϣ
			emit DeleteEmit(StrMarcoLabel);

			// �괦�������
			MarcoTableUpdate();

			// ѡ���Ÿ�ֵ
			iSelRowNo = NO_SELECTION;

			// �괦��ܵ���ʾ�������
			MarcoPipeUpdate();

			// ���ٰ�ť����
			FastBtnUpdate();

			break;
		}

		// ���ٴ���ť
		case ButtonType_FastBtn:
		{
			// ���ٰ�ť�ź�
			emit FastBtnEmit(StrMarcoLabel);

			// �괦�������
			MarcoTableUpdate();

			// ѡ����
			pMarcoTable->selectRow(iSelRowNo);

			// ���ٰ�ť����
			FastBtnUpdate();

			break;
		}

		default: break;
		
	}
	
	return;
}

/**************************************************************
�������� SelChangeSlot

���� ��  ѡ���иı�

��ڲ��� ��

��������ֵ ��

****************************************************************/
void LoadMarcoWidget::SelChangeSlot()
{
	// ��������
	int iRowNo;

	// ��ֵ��ȡ
	iRowNo = pMarcoTable->selectedItems()[0]->row();

	// ��ֵ�ж�
	if ((iSelRowNo == iRowNo) 
		|| (0 > iRowNo)
		|| (pMarco.get()->GetMarcoSize() <= iRowNo))
	{
		return;
	}

	// ѡ���и�ֵ
	iSelRowNo = iRowNo;

	// ���MarcoPipeArea��ʾ
	MarcoPipeUpdate();
	
	return;
}

/********************* LoadMarcoWidget END **************************/
