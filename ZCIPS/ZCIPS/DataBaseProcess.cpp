#include "DataBaseProcess.h"
/*****************************************************************
函数名：ImageDataBase

定义：构造函数

入口参数：

函数返回值：
*********************************************************************/
DataBaseProcess::DataBaseProcess()
{

}
DataBaseProcess::~DataBaseProcess()
{
}
/********************************************************************
函数名：ConnectDB

定义：连接数据库

入口参数：

函数返回值：
        bool ：   true -- 连接成功
		          false -- 连接失败
*/
bool DataBaseProcess::ConnectDB()
{
	//设置数据库类型
	db = QSqlDatabase::addDatabase("QSQLITE");
	//设置数据库名称
	db.setDatabaseName("ImageInformation.db3");
	//打开数据库
	if (!db.open())
	{
		return false;
	}
	QSqlQuery query(db);
	//判断数据库中是否存在表ImgTable *表名可更改
	bool IsTableExist = query.exec(QObject::tr("select count(*) from sqlite_master where type = 'table' and name = '%1'").arg("ImgTable"));
	//若数据库中不存在该表，则创建该表
	int m = query.size();
	if (!IsTableExist)
	{
		//创建表格时需要将要保存的所有条目添加进去
		query.exec(QObject::tr("create table ImgTable(名称 nvarchar(128) PRIMARY KEY,检测日期 datetime, \
                                检测人员 nvarchar(128),工件名称 nvarchar(128),文件是否存在 nvarchar(32))"));
	}
	model = new QSqlTableModel;
	model->setTable("ImgTable");
	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model->select();
	return true;

}
/************************************************************************
函数名：DisConnectDB

定义：断开数据库连接

入口参数：

函数返回值：
*************************************************************************/
void DataBaseProcess::DisConnectDB()
{
	delete model;
	db.close();
}
/************************************************************************
函数名：InsertDBdata

定义：向数据库中插入数据

入口参数：

函数返回值：  
         bool:    true -- 数据成功录入数据库
		          false -- 数据录入失败
**********************************************************************/
bool DataBaseProcess::InsertDBdata()
{
	//根据实际需要录入的数据进行修改
	QSqlQuery query(db);
	QDate date(QDate::currentDate());
	query.prepare(QObject::tr("insert into ImgTable values(:名称,:检测日期,:检测人员,:工件名称,:文件是否存在)"));
	query.addBindValue("C:/Users/yyq/Desktop/tif/0005.tif");
	query.addBindValue(date);
	query.addBindValue("小李");
	query.addBindValue("工件");
	query.addBindValue("是");
	bool result = query.exec();
	if (!result)
	{
		return false;
	}
	return true;
}
/*************************************************************
函数名：CreateImgLst

函数定义：获取文件夹中所包含的文件名

入口参数：

函数返回值：
           bool:          true -- 获取文件名成功
		                  false -- 获取文件名失败
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
函数名：CreateReclst

函数定义：获取数据库中记录的包含文件路径的文件名

入口参数：

函数返回值：
         bool：       true -- 获取成功
		              false -- 获取失败
**********************************************************************/
bool DataBaseProcess::CreateReclst()
{
	QVector <RecordFilePara> pNullVector;
	Reclst.swap(pNullVector);
	QSqlQuery query(db);
	bool result = query.exec(QObject::tr("select 名称 from ImgTable"));
	//获取数据库中条目总数
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
函数名：CompareLst

函数定义：找出图像列表和数据库列表中的相同部分

入口参数：

函数返回值：
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
函数名：SetRecImgState

函数定义：标记数据库中图像文件不存在的记录

入口参数：

函数返回值：
***************************************************************/
void DataBaseProcess::SetRecImgState()
{
	QSqlQuery query(db);	
	for (int i = 0; i < Reclst.count(); i++)
	{
		if (Reclst[i].mbSame == false)
		{
			 query.exec(QObject::tr("update ImgTable set 文件是否存在 = '%1' where 名称 ='%2'").arg("否").arg(Reclst[i].RecName));		
		}
	}
}
/**************************************************************
函数名：UpdateDBdata()

函数定义：将数据库中文件不存在项的文件是否存在设置为否

入口参数：

函数返回值：
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
函数名：DeleteDBdata

函数定义：删除数据库记录

入口参数：

函数返回值：
*************************************************************/
void DataBaseProcess::DeleteDBdata(int index)
{
	model->removeRow(index);
	//model->submitAll();
}
/*********************************************************
函数名：SubmitDBdata

函数定义：提交修改到数据库

入口参数：

函数返回值：
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
