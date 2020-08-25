#pragma once

#include <QtWidgets>
#include "Marco.h"

/***************************** �� ******************************/
#define FASTBTN_UNIT_WIDTH    60    // ���ٰ�ť���
#define FASTBTN_UNIT_HEIGHT   55    // ���ٰ�ť�߶�
#define FASTBTN_MARGIN        10    // ��ʾ�����Ե

/***************************** �� ******************************/
// ���ٰ�ť��
class FastButton : public QPushButton
{
    Q_OBJECT

// ��Ӧ������
public:
	FastButton(QWidget *parent = nullptr);

// �ۺ���
public slots:

	// �����¼��ۺ���
	void FastBtnClick(bool bIsChecked);

// �ź�
signals:

	// ��ݰ�ť�������
	void FastBtn(QString StrLabel);

};

// ���ٰ�ť������
class FastBtnArea : public QScrollArea
{

	Q_OBJECT

// ��Ӧ������
public:
	
	// ��ʼ��
	FastBtnArea(QWidget *parent = nullptr);

// ����
public:

	// ���
	void Clear();

	// ���ٰ�ť�������
	void FastBtnUpdate(QSharedPointer<Marco> pInMarco);

	// ���ذ�ť�ؼ�Vec
	QVector<FastButton *> GetBtnVec();

// �ؼ�
private:
	QVector<FastButton *> vecFastBtn;   // ��ťָ��Vec
	QGridLayout *pBtnLayout;			// ��ťLayout

};
