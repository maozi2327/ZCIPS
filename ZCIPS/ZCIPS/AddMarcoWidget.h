#pragma once

#include <QtWidgets>
#include "MarcoPipeArea.h"
#include "Marco.h"
#include "ImageInstance.h"
#include "ERRInfo.h"

class AddMarcoWidget : public QWidget
{
	Q_OBJECT

// ��������Ӧ
public:
	AddMarcoWidget
	(
		QSharedPointer<Marco> pInMarco,
		QSharedPointer<ImageInstance> pInImageIns,
		QWidget* parent = nullptr
	);

// ����
public:
	QSharedPointer<Marco> pMarco;
	QSharedPointer<ImageInstance> pImageIns;

// �ؼ�
public:
	QGridLayout *pLayout;        // ������
	QToolButton *pBtnAdd;        // ���Ӱ�ť
	MarcoPipeArea *pPipArea;     // ����ܵ�
	QLineEdit *pEdit;            // �༭�ؼ�
	QLabel *pLabel;              // ������ʾ�ؼ� 

// ����
public:

	// �����ʼ��
	void InitUI();

	// ���ش���ܵ���Ϣ
	void LoadProPipeInfo();

// �ۺ���
public slots:

	// ���ɺ���
	void GenerateSlot();

};