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

// 参数
public:

	QMap<QString, QSharedPointer<ImageProPipe>> MapProPipe;	// 图像处理管道Map
															// QString -- 图像标识
															// ImageProPipe -- 图像实例

	QList<QString> SortList;            // 因为Map无法排序，
										// 加入一个List容器来排序

	QMap<QString, bool> MapIsBtn;       // 是否存在快捷按钮Map
										// QString -- 图像标识
										// bool -- 是否存在快捷按钮

// 函数
public:

	// 获取Map的长度
	int Size();

	// 将图像处理管道插入Map
	void Insert
	(
		QString StrLabel,
		QSharedPointer<ImageProPipe> pInProPipe
	);

	// 获取对应编号的Key值
	QString KeyAt(int iNo);

	// 获取对应编号的Value值
	QSharedPointer<ImageProPipe> ValueAt(int iNo);

	// Value值搜寻
	QSharedPointer<ImageProPipe> Find(QString StrKey);

	// 设置是否存在快捷按钮
	void setBtn(QString StrLabel, bool bIsBtn);

	// 查询是否存在快捷按钮
	bool IsBtn(QString StrLabel);

	// 查询是否存在快捷按钮
	bool IsBtn(int iNo);

	// 清除
	void Clear();

	// 删除指定的处理管道
	void DeleteProPipe(QString StrLabel);
};


// 图像处理宏类
class Marco
{

// 参数
private:
	MapMarco m_MapMarco;		// 宏处理宏集合
								// QString -- 处理管道标识
								// QSharedPointer<ImageProPipe> -- 处理管道	

	QSqlDatabase m_Database;	// 数据库实例

	bool bIsConnected;

// 响应与重载
public:
	Marco();

// 函数
public:

	// 链接数据库
	bool ConnectDB();

	// 删除数据库
	void DeleteDB();

	// 清除宏
	void DeleteMarcoMap();

	// 依据宏更新数据库
	void UpdateDB();

	// 依据数据库更新宏
	void UpdateMarcoFromDB();

	// 插入处理管道
	void InsertProPipe
	(
		QString StrLabel,
		QSharedPointer<ImageProPipe> pImgProPipe
	);

	// 返回是否链接成功
	bool isConnectDataBase();

	// 处理解码
	int ParseProcess
	(
		QString StrProcessName,
		QString StrProcessPara,
		ProcessType &m_PType,
		ImageProcess **pImageProcess
	);

	// 获取用于宏数据库存储的参数字符（灰度变换）
	void GetMarcoDBParaString_GrayTrans
	(
		QString &StrPara,
		float fBottom,
		float fTop,
		float fR
	);

	// 解码灰度变换参数
	int ParseGrayTransPara
	(
		QString StrPara, 
		GrayTransProcess *pGrayTransPro
	);

	// 获取宏处理标识
	QString GetMarcoLabel(int iMarcoNo);

	// 获取宏长度
	int GetMarcoSize();

	// 当前宏是否存在快速按钮
	bool IsFastBtn(int iMarcoNo);

	// 当前宏是否存在快速按钮
	bool IsFastBtn(QString StrMarcoLabel);

	// 设置快速按钮
	void SetFastBtn(QString StrMarcoLabel, bool bIsFastBtn);

	// 获取图像处理管道（宏处理）
	QSharedPointer<ImageProPipe> GetImageProPipe(QString StrMarcoLabel);

	// 删除宏处理
	void DeleteMarco(QString StrMarcoLabel);
};

