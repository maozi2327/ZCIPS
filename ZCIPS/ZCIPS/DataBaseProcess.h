#pragma once
#pragma execution_character_set("utf-8")
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDate>
#include <QDir>

/********************************  �ṹ������  *****************************/
typedef struct tagRecordFilePara {
	QString RecName;                     //���ݿ��¼�ļ���
	bool mbSame;                        // ����ļ����Ƿ���ͬ
}RecordFilePara;
/*********************************  ��  ***********************************/
class DataBaseProcess
{
public:
	DataBaseProcess();
	~DataBaseProcess();
	//����
public:
	QSqlDatabase db;
	QSqlTableModel *model;
	QVector<RecordFilePara>Reclst;
	QVector<RecordFilePara>Imglst;
	QString sDBFilePath;                //¼�����ݿ����ļ����·��
//����
public:
	bool ConnectDB();                  //�������ݿ�
	void DisConnectDB();               //�Ͽ����ݿ�
	bool InsertDBdata();               //��������
	void UpdateDBdata();               //�������ݿ�
	bool CreateImgLst();               //�����ļ�����ͼ���б�
	bool CreateReclst();               //�������ݿ��м�¼�б�
	void CompareLst();                 //�Ƚ�ͼ���б�����ݿ��¼�б�
	void SetRecImgState();             //�����ݿ����ļ������ڵļ�¼���б��
	void DeleteDBdata(int index);      //ɾ�����ݿ��¼
	bool SubmitDBdata();               //�ύ�޸ĵ����ݿ�
};

