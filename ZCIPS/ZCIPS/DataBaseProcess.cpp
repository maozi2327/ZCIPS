#include "DataBaseProcess.h"
/*****************************************************************
��������ImageDataBase

���壺���캯��

��ڲ�����

��������ֵ��
*********************************************************************/
DataBaseProcess::DataBaseProcess()
{

}
DataBaseProcess::~DataBaseProcess()
{
}
/********************************************************************
��������ConnectDB

���壺�������ݿ�

��ڲ�����

��������ֵ��
        bool ��   true -- ���ӳɹ�
		          false -- ����ʧ��
*/
bool DataBaseProcess::ConnectDB()
{
	//�������ݿ�����
	db = QSqlDatabase::addDatabase("QSQLITE");
	//�������ݿ�����
	db.setDatabaseName("ImageInformation.db3");
	//�����ݿ�
	if (!db.open())
	{
		return false;
	}
	QSqlQuery query(db);
	//�ж����ݿ����Ƿ���ڱ�ImgTable *�����ɸ���
	bool IsTableExist = query.exec(QObject::tr("select count(*) from sqlite_master where type = 'table' and name = '%1'").arg("ImgTable"));
	//�����ݿ��в����ڸñ��򴴽��ñ�
	int m = query.size();
	if (!IsTableExist)
	{
		//�������ʱ��Ҫ��Ҫ�����������Ŀ��ӽ�ȥ
		query.exec(QObject::tr("create table ImgTable(���� nvarchar(128) PRIMARY KEY,������� datetime, \
                                �����Ա nvarchar(128),�������� nvarchar(128),�ļ��Ƿ���� nvarchar(32))"));
	}
	model = new QSqlTableModel;
	model->setTable("ImgTable");
	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model->select();
	return true;

}
/************************************************************************
��������DisConnectDB

���壺�Ͽ����ݿ�����

��ڲ�����

��������ֵ��
*************************************************************************/
void DataBaseProcess::DisConnectDB()
{
	delete model;
	db.close();
}
/************************************************************************
��������InsertDBdata

���壺�����ݿ��в�������

��ڲ�����

��������ֵ��  
         bool:    true -- ���ݳɹ�¼�����ݿ�
		          false -- ����¼��ʧ��
**********************************************************************/
bool DataBaseProcess::InsertDBdata()
{
	//����ʵ����Ҫ¼������ݽ����޸�
	QSqlQuery query(db);
	QDate date(QDate::currentDate());
	query.prepare(QObject::tr("insert into ImgTable values(:����,:�������,:�����Ա,:��������,:�ļ��Ƿ����)"));
	query.addBindValue("C:/Users/yyq/Desktop/tif/0005.tif");
	query.addBindValue(date);
	query.addBindValue("С��");
	query.addBindValue("����");
	query.addBindValue("��");
	bool result = query.exec();
	if (!result)
	{
		return false;
	}
	return true;
}
/*************************************************************
��������CreateImgLst

�������壺��ȡ�ļ��������������ļ���

��ڲ�����

��������ֵ��
           bool:          true -- ��ȡ�ļ����ɹ�
		                  false -- ��ȡ�ļ���ʧ��
*************************************************************/
bool DataBaseProcess::CreateImgLst()
{
	QVector <RecordFilePara> pNullVector;
	Imglst.swap(pNullVector);
	QDir dir(sDBFilePath);
	if (!dir.exists())
	{
		return false;
	}
	dir.setFilter(QDir::Dirs | QDir::Files);
	QFileInfoList fileinfolst = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
	QFileInfo fileinfo;
	RecordFilePara tempImg;
	for (int i = 0; i < fileinfolst.count(); i++)
	{
		fileinfo = fileinfolst.at(i);
		if (fileinfo.isFile() && (fileinfo.suffix()== "tif"))
		{
			tempImg.RecName = fileinfo.filePath();
			tempImg.mbSame = false;
			Imglst.push_back(tempImg);
		}
		else if (fileinfo.isDir())
		{
			tempImg.RecName = fileinfo.filePath();
			tempImg.mbSame = false;
			Imglst.push_back(tempImg);
		}
	}
	return true;
}
/***********************************************************************
��������CreateReclst

�������壺��ȡ���ݿ��м�¼�İ����ļ�·�����ļ���

��ڲ�����

��������ֵ��
         bool��       true -- ��ȡ�ɹ�
		              false -- ��ȡʧ��
**********************************************************************/
bool DataBaseProcess::CreateReclst()
{
	QVector <RecordFilePara> pNullVector;
	Reclst.swap(pNullVector);
	QSqlQuery query(db);
	bool result = query.exec(QObject::tr("select ���� from ImgTable"));
	//��ȡ���ݿ�����Ŀ����
	int rowcount = query.size();
	if (!result)
	{
		return false;
	}
	//QString tempname;
	RecordFilePara tempImg;
	while (query.next())
	{
		tempImg.RecName = query.value(0).toString();
		tempImg.mbSame = false;
		Reclst.push_back(tempImg);
	}
	return true;
}
/**************************************************************************
��������CompareLst

�������壺�ҳ�ͼ���б�����ݿ��б��е���ͬ����

��ڲ�����

��������ֵ��
****************************************************************************/
void DataBaseProcess::CompareLst()
{
	int ImgCount = Imglst.count();
	int RecCount = Reclst.count();
	for (int i = 0; i < RecCount; i++)
	{
		for (int j = 0; j < ImgCount; j++)
		{
			if (Imglst[j].RecName == Reclst[i].RecName)
			{
				Imglst[j].mbSame = true;
				Reclst[i].mbSame = true;
				
			}
		}
	}
}
/*****************************************************************
��������SetRecImgState

�������壺������ݿ���ͼ���ļ������ڵļ�¼

��ڲ�����

��������ֵ��
***************************************************************/
void DataBaseProcess::SetRecImgState()
{
	QSqlQuery query(db);	
	for (int i = 0; i < Reclst.count(); i++)
	{
		if (Reclst[i].mbSame == false)
		{
			 query.exec(QObject::tr("update ImgTable set �ļ��Ƿ���� = '%1' where ���� ='%2'").arg("��").arg(Reclst[i].RecName));		
		}
	}
}
/**************************************************************
��������UpdateDBdata()

�������壺�����ݿ����ļ�����������ļ��Ƿ��������Ϊ��

��ڲ�����

��������ֵ��
****************************************************************/
void DataBaseProcess::UpdateDBdata()
{
	model->submitAll();
	CreateReclst();
	CreateImgLst();
	CompareLst();
	SetRecImgState();
}
/**************************************************************
��������DeleteDBdata

�������壺ɾ�����ݿ��¼

��ڲ�����

��������ֵ��
*************************************************************/
void DataBaseProcess::DeleteDBdata(int index)
{
	model->removeRow(index);
	//model->submitAll();
}
/*********************************************************
��������SubmitDBdata

�������壺�ύ�޸ĵ����ݿ�

��ڲ�����

��������ֵ��
*********************************************************/
bool DataBaseProcess::SubmitDBdata()
{
	model->database().transaction();
	if (model->submitAll())
	{
		model->database().commit();
		return true;
	}
	else {
		model->database().rollback();
		return false;
	}
}
