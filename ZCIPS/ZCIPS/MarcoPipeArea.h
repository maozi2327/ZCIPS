#pragma once

#include <QtWidgets>

/***************************** �� ******************************/
#define MARCOPIPE_NO_SELECT			  -1    // ��ǰ��ѡ��
#define MARCOPIPLE_UNIT_WIDTH         100   // �괦��Ԫ��ʾ���


/***************************** �� ******************************/
class MarcoPipeArea : public QScrollArea
{
	Q_OBJECT

// ����
private:
	int iSelNo;								// ��ǰѡ�е�Widget���
	QVector<QTableWidget *> vecPipeWidget;  // �ڲ���Widget�б�
	QGridLayout *pPipeLayout;               // ����ܵ�Layout

// ��Ӧ������
public:

	// ��ʼ��
	MarcoPipeArea(QWidget *parent = nullptr);

// �ۺ���
public slots:

	// ������ۺ���
	void MousePressSlot(int iRow, int iCollum);

// ����
public:

	// ��Ӵ���ܵ���ʾ����
	void AddPipeUnit
	(
		QString StrProcessName,
		QString StrProcessContent
	);

	// �����������ܵ�
	void ClearPipe();

	// ��ȡ��ǰѡ����
	int GetCurrSel();

	// �ӵ�ѡ��λ�ýض�
	void CutFromCurrSel();

	// ѡ�е�ǰ��Widget
	void SelectWidget();
};