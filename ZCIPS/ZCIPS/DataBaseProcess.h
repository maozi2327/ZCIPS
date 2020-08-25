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

/********************************  结构体类型  *****************************/
typedef struct tagRecordFilePara {
	QString RecName;                     //数据库记录文件名
	bool mbSame;                        // 标记文件名是否相同
}RecordFilePara;
/*********************************  类  ***********************************/
class DataBaseProcess
{
public:
	DataBaseProcess();
	~DataBaseProcess();
	//参数
public:
	QSqlDatabase db;
	QSqlTableModel *model;
	QVector<RecordFilePara>Reclst;
	QVector<RecordFilePara>Imglst;
	QString sDBFilePath;                //录入数据库中文件存放路径
//函数
public:
	bool ConnectDB();                  //连接数据库
	void DisConnectDB();               //断开数据库
	bool InsertDBdata();               //插入数据
	void UpdateDBdata();               //更新数据库
	bool CreateImgLst();               //创建文件夹中图像列表
	bool CreateReclst();               //创建数据库中记录列表
	void CompareLst();                 //比较图像列表和数据库记录列表
	void SetRecImgState();             //将数据库中文件不存在的记录进行标记
	void DeleteDBdata(int index);      //删除数据库记录
	bool SubmitDBdata();               //提交修改到数据库
};

