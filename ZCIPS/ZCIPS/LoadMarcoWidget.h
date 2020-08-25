#pragma once

#include <QtWidgets>
#include "ImageInstance.h"
#include "Marco.h"
#include "FastBtnArea.h"
#include "MarcoPipeArea.h"

/*********************** �� **************************/
#define NO_SELECTION   -1      // ����δѡ��Ϊ-1

/*********************** ö������ **************************/
// ��ť����
enum enButtonType
{
	ButtonType_Excute = 0,
	ButtonType_Delete,
	ButtonType_FastBtn
};

/*********************** �� **************************/
// ���ť��
class TableButton : public QPushButton
{
	Q_OBJECT

// ��Ӧ������
public:

	// ���캯��
	TableButton
	(
		enButtonType BInType,
		QString StrInMarcoLabel,
		QWidget *parent = nullptr
	);

// �ۺ���
public slots:
	
	// �����ۺ���
	void ClickSlot(bool bChecked);

// �ź�
signals:

	// �����ź�
	void ClickEmit(enButtonType, QString);


// ���� 
private:
	enButtonType m_BType;
	QString StrMarcoLabel;

};


// ����/�༭�괦����
class LoadMarcoWidget : public QWidget
{
	Q_OBJECT

// ��������Ӧ
public:
	LoadMarcoWidget
	(
		QSharedPointer<ImageInstance> pInImageIns,
		QSharedPointer<Marco> pInMarco,
		QWidget *parent = nullptr
	);

// ����
private:
	int iSelRowNo;                               // ��ǰѡ����
	QSharedPointer<ImageInstance> pImageIns;
	QSharedPointer<Marco> pMarco;

// �ؼ�
private:
	QTableWidget *pMarcoTable;
	FastBtnArea *pFastBtnArea;
	MarcoPipeArea *pMarcoPipeArea;
	QGridLayout *pLayout;

// �ۺ���
public slots:

	// ���ť�����ۺ���
	void TblBtnClickSlot
	(
		enButtonType BType, 
		QString StrMarcoLabel
	);

	// ��ѡ����Ĳۺ���
	void SelChangeSlot();

// �ź�
signals:
	
	// ִ�к괦���ź�
	void ExcuteEmit(QString StrMarcoLabel);

	// ɾ���괦���ź�
	void DeleteEmit(QString StrMarcoLabel);

	// ���ٰ�ť�����ź�
	void FastBtnEmit(QString StrMarcoLabel);

// ����
public:

	// ��ʼ��
	void InitUI();

	// �������
	void MarcoTableUpdate();

	// �괦��ܵ���ʾ����
	void MarcoPipeUpdate();

	// ���ٰ�ť��ʾ����
	void FastBtnUpdate();
};

