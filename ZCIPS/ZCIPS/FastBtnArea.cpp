
#include "FastBtnArea.h"

/********************* FastButton START **********************/
/**************************************************************
�������� FastButton

���� ��  ���캯��

��ڲ��� ��
	QWidget *parent ��	��ָ��

��������ֵ ��

****************************************************************/
FastButton::FastButton(QWidget *parent)
	: QPushButton(parent)
{
	// �ۺ������ź�����
	connect(this, SIGNAL(clicked(bool)), 
		this, SLOT(FastBtnClick(bool)));
}

/**************************************************************
�������� FastBtnClick

���� ��  �����¼��ۺ���

��ڲ��� ��
	bool bIsChecked : 

��������ֵ ��

****************************************************************/
void FastButton::FastBtnClick(bool bIsChecked)
{
	// ��������
	QString StrLabel;

	// ��ȡ��ʾ��ǩ
	StrLabel = this->text();
	
	// �����ź�
	emit FastBtn(StrLabel);

	return;
}

/********************* FastButton END **********************/





/********************* FastBtnArea START **********************/

/**************************************************************
�������� FastBtnArea

���� ��  ���캯��

��ڲ��� ��
	QWidget *parent ��					��ָ��

��������ֵ ��

****************************************************************/
FastBtnArea::FastBtnArea(QWidget *parent)
	: QScrollArea(parent)
{
	// ��������
	bool bIsBtn;			// �Ƿ�Ϊ���ٴ���ť
	QWidget *pFastBtnAreaWidget;   

	// ����Widget
	pFastBtnAreaWidget = new QWidget();
	pFastBtnAreaWidget->setObjectName(QStringLiteral("FastBtnAreaWidget"));
	this->setWidget(pFastBtnAreaWidget);
	pFastBtnAreaWidget->setContentsMargins(0, 0, 0, 0);

	// FastBtnArea����
	this->setObjectName(QStringLiteral("FastBtnArea"));
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	this->setWidgetResizable(true);

	// Layout��ʼ��
	pBtnLayout = new QGridLayout(this->widget());
	pBtnLayout->setContentsMargins(0, 0, 0, 0);
}

/**************************************************************
�������� FastBtnUpdate

���� ��  ���ٰ�ť�������

��ڲ��� ��
	QSharedPointer<Marco> pInMarco ��	�괦�����ָ��

��������ֵ ��

****************************************************************/
void FastBtnArea::FastBtnUpdate(QSharedPointer<Marco> pInMarco)
{
	// ��������	
	int iBtnSize;
	int iMarcoSize;  		// ������
	QString StrMarcoLabel;  // �괦��ܵ���ʶ
	FastButton *pFastBtn;   // ���ٰ�ť�ؼ�ָ��

	// ������ٰ�ť����
	Clear();

	// ������ֵ
	iBtnSize = 0;
	iMarcoSize = pInMarco.get()->GetMarcoSize();

	// ��ť����
	for (int i = 0; i < iMarcoSize; i++)
	{
		// �ж��Ƿ���ڿ��ٰ�ť
		// ���ڣ�����������ٰ�ť����
		if (true == pInMarco.get()->IsFastBtn(i))
		{
			// ��ӿ��ٰ�ť
			pFastBtn = new FastButton();

			// ���ð�ť����
			pFastBtn->setFixedWidth(FASTBTN_UNIT_WIDTH);
			pFastBtn->setFixedHeight(FASTBTN_UNIT_HEIGHT);
			StrMarcoLabel = pInMarco.get()->GetMarcoLabel(i);
			pFastBtn->setText(StrMarcoLabel);

			// ����ť��ӽ�Layout
			pBtnLayout->addWidget(pFastBtn, 0, iBtnSize);

			// ����ťָ������vec
			vecFastBtn.push_back(pFastBtn);

			// ��ť������������
			iBtnSize++;
		}
	}

	// ����Widget���
	this->widget()->setFixedWidth(
		vecFastBtn.size() * FASTBTN_UNIT_WIDTH + FASTBTN_MARGIN);

	return;
}

/**************************************************************
�������� Clear

���� ��  ������ٰ�ť

��ڲ��� ��

��������ֵ ��

****************************************************************/
void FastBtnArea::Clear()
{
	// ��������
	int iFastBtnSize;
	FastButton *pFastBtn;
	// QTableWidget *pTableWidget;

	// ���ٰ�ť���Ȼ�ȡ
	iFastBtnSize = vecFastBtn.size();
	if (0 == iFastBtnSize) return;

	// ɾ�����ؽ������Widget
	while (0 != vecFastBtn.size())
	{
		pFastBtn = vecFastBtn[0];
		pBtnLayout->removeWidget(pFastBtn);
		vecFastBtn.pop_front();
		delete pFastBtn;
	}

	// �������
	this->widget()->setFixedWidth(0);

	return;
}

/**************************************************************
�������� GetBtnVec

���� ��  ���ذ�ť�ؼ�Vec

��ڲ��� ��

��������ֵ ��

****************************************************************/
QVector<FastButton *> FastBtnArea::GetBtnVec()
{
	return vecFastBtn;
}

/********************* FastBtnArea END **********************/