#pragma once

#include <QtWidgets>
#include "ImageProcess.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDate>



class MapMarco
{

// ����
public:

	QMap<QString, QSharedPointer<ImageProPipe>> MapProPipe;	// ͼ����ܵ�Map
															// QString -- ͼ���ʶ
															// ImageProPipe -- ͼ��ʵ��

	QList<QString> SortList;            // ��ΪMap�޷�����
										// ����һ��List����������

	QMap<QString, bool> MapIsBtn;       // �Ƿ���ڿ�ݰ�ťMap
										// QString -- ͼ���ʶ
										// bool -- �Ƿ���ڿ�ݰ�ť

// ����
public:

	// ��ȡMap�ĳ���
	int Size();

	// ��ͼ����ܵ�����Map
	void Insert
	(
		QString StrLabel,
		QSharedPointer<ImageProPipe> pInProPipe
	);

	// ��ȡ��Ӧ��ŵ�Keyֵ
	QString KeyAt(int iNo);

	// ��ȡ��Ӧ��ŵ�Valueֵ
	QSharedPointer<ImageProPipe> ValueAt(int iNo);

	// Valueֵ��Ѱ
	QSharedPointer<ImageProPipe> Find(QString StrKey);

	// �����Ƿ���ڿ�ݰ�ť
	void setBtn(QString StrLabel, bool bIsBtn);

	// ��ѯ�Ƿ���ڿ�ݰ�ť
	bool IsBtn(QString StrLabel);

	// ��ѯ�Ƿ���ڿ�ݰ�ť
	bool IsBtn(int iNo);

	// ���
	void Clear();

	// ɾ��ָ���Ĵ���ܵ�
	void DeleteProPipe(QString StrLabel);
};


// ͼ�������
class Marco
{

// ����
private:
	MapMarco m_MapMarco;		// �괦��꼯��
								// QString -- ����ܵ���ʶ
								// QSharedPointer<ImageProPipe> -- ����ܵ�	

	QSqlDatabase m_Database;	// ���ݿ�ʵ��

	bool bIsConnected;

// ��Ӧ������
public:
	Marco();

// ����
public:

	// �������ݿ�
	bool ConnectDB();

	// ɾ�����ݿ�
	void DeleteDB();

	// �����
	void DeleteMarcoMap();

	// ���ݺ�������ݿ�
	void UpdateDB();

	// �������ݿ���º�
	void UpdateMarcoFromDB();

	// ���봦��ܵ�
	void InsertProPipe
	(
		QString StrLabel,
		QSharedPointer<ImageProPipe> pImgProPipe
	);

	// �����Ƿ����ӳɹ�
	bool isConnectDataBase();

	// �������
	int ParseProcess
	(
		QString StrProcessName,
		QString StrProcessPara,
		ProcessType &m_PType,
		ImageProcess **pImageProcess
	);

	// ��ȡ���ں����ݿ�洢�Ĳ����ַ����Ҷȱ任��
	void GetMarcoDBParaString_GrayTrans
	(
		QString &StrPara,
		float fBottom,
		float fTop,
		float fR
	);

	// ����Ҷȱ任����
	int ParseGrayTransPara
	(
		QString StrPara, 
		GrayTransProcess *pGrayTransPro
	);

	// ��ȡ�괦���ʶ
	QString GetMarcoLabel(int iMarcoNo);

	// ��ȡ�곤��
	int GetMarcoSize();

	// ��ǰ���Ƿ���ڿ��ٰ�ť
	bool IsFastBtn(int iMarcoNo);

	// ��ǰ���Ƿ���ڿ��ٰ�ť
	bool IsFastBtn(QString StrMarcoLabel);

	// ���ÿ��ٰ�ť
	void SetFastBtn(QString StrMarcoLabel, bool bIsFastBtn);

	// ��ȡͼ����ܵ����괦��
	QSharedPointer<ImageProPipe> GetImageProPipe(QString StrMarcoLabel);

	// ɾ���괦��
	void DeleteMarco(QString StrMarcoLabel);
};

