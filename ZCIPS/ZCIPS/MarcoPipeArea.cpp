#include "MarcoPipeArea.h"

/**************************************************************
�������� MarcoPipeArea

���� ��  ���캯��

��ڲ��� ��
	QWidget *parent �� ��ָ��

��������ֵ ��

****************************************************************/
MarcoPipeArea::MarcoPipeArea(QWidget* parent)
	: QScrollArea(parent)
{
	// ��������
	QWidget *pMarcoPipeAreaWidget;
	
	// ������ʼ��
	iSelNo = MARCOPIPE_NO_SELECT;

	// ����Widget
	pMarcoPipeAreaWidget = new QWidget();
	pMarcoPipeAreaWidget->setObjectName(QStringLiteral("MarcoPipeAreaWidget"));
	this->setWidget(pMarcoPipeAreaWidget);

	// MarcoPipeArea����
	this->setObjectName(QStringLiteral("MarcoPipeArea"));
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	this->setWidgetResizable(true);

	// �ؼ���ʼ��
	pPipeLayout = new QGridLayout(this->widget());
}

/**************************************************************
�������� MousePressSlot

���� ��  ������¼��ۺ���

��ڲ��� ��
	int iRow ��    ��ֵ
	int iCollum �� ��ֵ

��������ֵ ��

****************************************************************/
void MarcoPipeArea::MousePressSlot(int iRow, int iCollum)
{
	// ��������Widget
	for (int i = 0; i < vecPipeWidget.size(); i++)
	{
		if (vecPipeWidget[i]->underMouse())
		{			
			// ��ǰѡ���Ÿ�ֵ
			iSelNo = i;
	
			break;
		}
	}

	// ��Ե����
	for (int i = 0; i < vecPipeWidget.size(); i++)
	{
		if (i == iSelNo) 
			vecPipeWidget[i]->setStyleSheet("background-color:white;border: 2px solid #33CCFF;");
		else
			vecPipeWidget[i]->setStyleSheet("background-color:white;border: 1px solid #C4C4C3;");
	}
}

/**************************************************************
�������� AddPipeUnit

���� ��  ��Ӵ���ܵ���ʾ����

��ڲ��� ��
	QString StrProcessName  ��		������
	QString StrProcessContent  ��	��������

��������ֵ ��

****************************************************************/
void MarcoPipeArea::AddPipeUnit
(
	QString StrProcessName,
	QString StrProcessContent
)
{
	// ��������
	int iLastUnitPos;
	QTableWidget *pTblWidget;
	QTableWidgetItem *pTableItem;

	// ��ʼ��
	pTblWidget = new QTableWidget();

	// ����������
	pTblWidget->setRowCount(2);
	pTblWidget->setColumnCount(1);
	pTblWidget->setColumnWidth(0, MARCOPIPLE_UNIT_WIDTH);
	pTblWidget->verticalHeader()->setVisible(false);
	pTblWidget->horizontalHeader()->setVisible(false);
	pTblWidget->setWordWrap(true);
	pTblWidget->setStyleSheet("background-color:white;border: 1px solid #C4C4C3;");

	// �����������
	pTableItem = new QTableWidgetItem;
	pTableItem->setText(StrProcessName);
	pTblWidget->setItem(0, 0, pTableItem);
	pTableItem = new QTableWidgetItem;
	pTableItem->setText(StrProcessContent);
	pTblWidget->setItem(1, 0, pTableItem);
	pTblWidget->resizeRowsToContents();
	pTblWidget->setMouseTracking(true);
	
	// �������
	connect(pTblWidget, SIGNAL(cellClicked(int, int)), this, SLOT(MousePressSlot(int, int)));

	// ��QWidget�������
	vecPipeWidget.push_back(pTblWidget);

	// ��������������
	iLastUnitPos = vecPipeWidget.size() - 1;
	pPipeLayout->addWidget(pTblWidget, 0, iLastUnitPos);

	// �������
	this->widget()->setFixedWidth(vecPipeWidget.size() * MARCOPIPLE_UNIT_WIDTH);

	return;
}

/**************************************************************
�������� ClearPipe

���� ��  �����������ܵ�

��ڲ��� ��

��������ֵ ��

****************************************************************/
void MarcoPipeArea::ClearPipe()
{
	// ��������
	int iPipleSize;
	QTableWidget *pTableWidget;

	// ����ܵ����Ȼ�ȡ
	iPipleSize = vecPipeWidget.size();
	if (0 == iPipleSize) return;

	// ɾ�����ؽ������Widget
	while (0 != vecPipeWidget.size())
	{
		pTableWidget = vecPipeWidget[0];
		pPipeLayout->removeWidget(pTableWidget);
		vecPipeWidget.pop_front();
		delete pTableWidget;
	}

	// �������
	this->widget()->setFixedWidth(0);

	return;
}

/**************************************************************
�������� GetCurrSel

���� ��  ��ȡ��ǰѡ����

��ڲ��� ��

��������ֵ ��
	int iSelNo : ��ǰѡ����
****************************************************************/
int MarcoPipeArea::GetCurrSel()
{
	return iSelNo;
}

/**************************************************************
�������� CutFromCurrSel

���� ��  �ӵ�ѡ��λ�ýض�

��ڲ��� ��

��������ֵ ��
	
****************************************************************/
void MarcoPipeArea::CutFromCurrSel()
{
	// ��������
	QTableWidget *pTblWidget;
	
	// ��ǰѡ���ż��
	if ((iSelNo == MARCOPIPE_NO_SELECT)
		|| (iSelNo >= vecPipeWidget.size()))
	{
		return;
	}

	// ɾ����Ӧ��Widget
	while (vecPipeWidget.size() > iSelNo)
	{
		pTblWidget = vecPipeWidget[iSelNo];
		pPipeLayout->removeWidget(pTblWidget);
		vecPipeWidget.removeAt(iSelNo);
		delete pTblWidget;
	}

	// ���ÿؼ����
	this->widget()->setFixedWidth(vecPipeWidget.size() * MARCOPIPLE_UNIT_WIDTH);

	// ѡ���Ÿ�ֵ
	iSelNo = MARCOPIPE_NO_SELECT;
}

/**************************************************************
�������� SelectWidget

���� ��  ѡ�е�ǰ��Widget

��ڲ��� ��

��������ֵ ��

****************************************************************/
void MarcoPipeArea::SelectWidget()
{
	// ��ǰѡ���ż��
	if ((iSelNo == MARCOPIPE_NO_SELECT)
		|| (iSelNo >= vecPipeWidget.size()))
	{
		return;
	}

	// ��Ե����
	for (int i = 0; i < vecPipeWidget.size(); i++)
	{
		if (i == iSelNo)
			vecPipeWidget[i]->setStyleSheet("background-color:white;border: 2px solid #33CCFF;");
		else
			vecPipeWidget[i]->setStyleSheet("background-color:white;border: 1px solid #C4C4C3;");
	}
}