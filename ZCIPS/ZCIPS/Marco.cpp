#include "Marco.h"
#include "ERRInfo.h"

/**************************************************************
函数名： Marco

定义 ：  构造函数

入口参数 ：

函数返回值 ：

****************************************************************/
Marco::Marco()
{
	// 链接数据库
	bIsConnected = ConnectDB();
	if (false == bIsConnected) return;
	
	// 载入宏
	UpdateMarcoFromDB();

}

/**************************************************************
函数名： ConnectDB

定义 ：  链接数据库

入口参数 ：

函数返回值 ：
	true ：  成功
	false ： 失败
****************************************************************/
bool Marco::ConnectDB()
{
	// 参数声明
	bool bRet;
	bool bIsExsit;
	QString StrDataBasePath;
	QSqlQuery m_Query;           // 查询实例
	
	//设置数据库类型
	m_Database = QSqlDatabase::addDatabase("QSQLITE");

	//设置数据库名称
	// 获取数据库文件路径
	StrDataBasePath = QCoreApplication::applicationDirPath();
	StrDataBasePath = StrDataBasePath + "\\Marco.db3";
	m_Database.setDatabaseName(StrDataBasePath);

	//打开数据库
	bRet = m_Database.open();
	if (false == bRet) return false;

	// 查询实例初始化
	m_Query = QSqlQuery(m_Database);

	// 判断表格是否存在
	// 不存在则创建表格
	m_Query.exec(QObject::tr("SELECT * FROM sqlite_master \
			WHERE type = 'table' AND tbl_name = 'MarcoTable'"));
	if (false == m_Query.next())
	{
		m_Query.exec(QObject::tr("CREATE TABLE MarcoTable \
		(ID INT PRIMARY KEY, \
		 MarcoName nvarchar(128), \
         FastButton BOOLEAN)"));
	}

	return true;
}

/**************************************************************
函数名： DeleteDB

定义 ：  删除数据库

入口参数 ：

函数返回值 ：
	
****************************************************************/
void Marco::DeleteDB()
{
	// 参数声明
	QString StrMarcoLabel;
	QString QuerySentence;
	QSqlQuery m_Query;
	QSqlQuery m_SubQuery;
	
	// 链接检查
	if (false == bIsConnected) return;
	
	// 将Query与数据库适配
	m_Query = QSqlQuery(m_Database);
	m_SubQuery = QSqlQuery(m_Database);

	// 读取宏主表MarcoTable
	m_Query.exec("SELECT * FROM MarcoTable");
	while (m_Query.next())
	{
		// 获取宏标识，同时也获取了宏子表名
		StrMarcoLabel = m_Query.value(1).toString();

		// 删除对应的宏子表
		QuerySentence.clear();
		QuerySentence = "DROP TABLE " + StrMarcoLabel;
		m_SubQuery.exec(QuerySentence);
	}
	
	// 清空宏主表
	m_Query.exec("DELETE FROM MarcoTable");  // 删除表格
	m_Query.exec("VACCUUM");				 // 释放内存空间

	return;
}

/**************************************************************
函数名： UpdateDB

定义 ：  依据宏更新数据库

入口参数 ：

函数返回值 ：
	
****************************************************************/
void Marco::UpdateDB()
{
	// 参数声明
	int iRet;
	int iBool;
	QString MarcoLabel;
	QString StrProcessName;
	QString StrProcessPara;
	QString StrSentence;
	QSqlQuery m_Query;
	QSharedPointer<ImageProPipe> pImageProPipe;
	ProcessType InType;
	ImageProcess *pImageProcess;

	// 链接检查
	if (false == bIsConnected) return;

	// 宏检查
	if (0 == m_MapMarco.Size()) return;

	// 删除表格
	DeleteDB();

	// 将Query与数据库适配
	m_Query = QSqlQuery(m_Database);

	// Marco -> DB
	for (int iKeyNo = 0; iKeyNo < m_MapMarco.Size(); iKeyNo++)
	{
		// 获取宏标识与处理
		MarcoLabel = m_MapMarco.KeyAt(iKeyNo);
		pImageProPipe = m_MapMarco.ValueAt(iKeyNo);
		if (true == m_MapMarco.IsBtn(iKeyNo)) iBool = 1;
		else iBool = 0;
		
		// 填写宏主表
		m_Query.prepare("INSERT INTO MarcoTable (ID, MarcoName, FastButton)"
			"VALUES (:ID, :MarcoName, :FastButton)");
		m_Query.bindValue(":ID", (iKeyNo + 1));
		m_Query.bindValue(":MarcoName", MarcoLabel);
		m_Query.bindValue(":FastButton", iBool);
		m_Query.exec();

		// 创建宏子表
		StrSentence.clear();
		StrSentence = "CREATE TABLE "
			+ MarcoLabel
			+ " ( ID INT PRIMARY KEY, "
			+ "ProcessName nvarchar(100), "
			+ "ProcessPara nvarchar(200))";
		m_Query.exec(StrSentence);

		// 填写宏子表
		for (int iProNo = 0; iProNo < pImageProPipe.get()->GetPipeLen(); iProNo++)
		{
			// 获取处理方式
			iRet = pImageProPipe.get()->PipeGet(iProNo, InType, &pImageProcess);
			if (SUCCESS != iRet) return;
			
			// 
			switch (InType)
			{
				// 无处理
				case ProcessType_None: return;

				// 灰度变换
				case ProcessType_GrayTrans:
				{
					// 获取处理名
					StrProcessName.clear();
					StrProcessName = "GrayTrans";
					
					// 获取处理参数
					GrayTransProcess *pGrayTransPro
						= (GrayTransProcess *)pImageProcess;
					StrProcessPara.clear();
					GetMarcoDBParaString_GrayTrans(StrProcessPara,
						pGrayTransPro->GetBottom(),
						pGrayTransPro->GetTop(),
						pGrayTransPro->GetR());

					break;
				}

				// 图像增强
				case ProcessType_Enhancement:
				{
					break;
				}

				default:
				{
					return;
				}
			}

			// 填写宏子表
			StrSentence.clear();
			StrSentence = "INSERT INTO "
				+ MarcoLabel
				+ " (ID, ProcessName, ProcessPara)"
				+ " VALUES (:ID, :ProcessName, :ProcessPara)";
			m_Query.prepare(StrSentence);
			m_Query.bindValue(":ID", iProNo + 1);
			m_Query.bindValue(":ProcessName", StrProcessName);
			m_Query.bindValue(":ProcessPara", StrProcessPara);
			m_Query.exec();
		}
	}
	
	return;
}

/**************************************************************
函数名： UpdateMarcoFromDB

定义 ：  依据数据库更新宏

入口参数 ：

函数返回值 ：

****************************************************************/
void Marco::UpdateMarcoFromDB()
{
	// 参数声明
	int iRet;
	bool bRet;
	QString StrMarcoLabel;
	QString StrProcessName;
	QString StrProcessPara;
	QString StrQuerySentence;
	QString StrIsFastBtn;
	QSqlQuery m_Query;
	QSqlQuery m_SubQuery;
	ProcessType m_PType;
	QSharedPointer<ImageProPipe> pProPipe;
	ImageProcess *pImageProcess;

	// 链接检查
	if (false == bIsConnected) return;
	
	// 清除宏
	DeleteMarcoMap();

	// 将Query与数据库适配
	m_Query = QSqlQuery(m_Database);
	m_SubQuery = QSqlQuery(m_Database);

	// 读取主表宏
	m_Query.exec(QObject::tr("select * from MarcoTable"));
	while (m_Query.next())
	{
		// 获取宏标识，同时也获取了宏子表名
		StrMarcoLabel = m_Query.value(1).toString();

		// 初始化处理管道
		pProPipe.reset(new ImageProPipe);

		// 读取子表宏
		StrQuerySentence = "SELECT * FROM " + StrMarcoLabel;
		m_SubQuery.exec(StrQuerySentence);
		while (m_SubQuery.next())
		{
			// 读取处理名
			StrProcessName.clear();
			StrProcessName = m_SubQuery.value(1).toString();

			// 读取处理参数
			StrProcessPara.clear();
			StrProcessPara = m_SubQuery.value(2).toString();

			// 解析图像处理
			iRet = ParseProcess(StrProcessName, StrProcessPara, m_PType, &pImageProcess);
			if (SUCCESS != iRet) return;

			// 加载入图像处理管道
			iRet = pProPipe.get()->PipeAppend(m_PType, pImageProcess);
			if (SUCCESS != iRet) return;
		}

		// 加载进入宏Map
		m_MapMarco.Insert(StrMarcoLabel, pProPipe);

		// 获取是否存在快速按钮
		StrIsFastBtn = m_Query.value(2).toString();
		if ("1" == StrIsFastBtn) 
			m_MapMarco.setBtn(StrMarcoLabel, true);
		else m_MapMarco.setBtn(StrMarcoLabel, false);
	}
}

/**************************************************************
函数名： InsertProPipe

定义 ：  插入处理管道到宏

入口参数 ：
	QString StrLabel ：标识
	QSharedPointer<ImageProPipe> pImgProPipe ： 

函数返回值 ：

****************************************************************/
void Marco::InsertProPipe
(
	QString StrLabel,
	QSharedPointer<ImageProPipe> pImgProPipe
)
{
	// 将处理管道插入宏中
	m_MapMarco.Insert(StrLabel, pImgProPipe);

	// 更新数据库
	UpdateDB();
	
	return;
}

/**************************************************************
函数名： DeleteMarcoMap

定义 ：  删除宏

入口参数 ：

函数返回值 ：

****************************************************************/
void Marco::DeleteMarcoMap()
{
	// 删除所有宏
	m_MapMarco.Clear();

	return;
}

/**************************************************************
函数名： isConnectDataBase

定义 ：  是否链接成功

入口参数 ：

函数返回值 ：
	true ：  成功
	false ： 失败
****************************************************************/
bool Marco::isConnectDataBase(){ return bIsConnected; }

/**************************************************************
函数名： isConnectDataBase

定义 ：  是否链接成功

入口参数 ：
	QString StrProcessName ：		
	QString StrProcessPara ：
	ProcessType &m_PType ：
	ImageProcess **pImageProcess ：

函数返回值 ：
	iRet :  0      -- 成功
	        others -- 错误代码
****************************************************************/
int Marco::ParseProcess
(
	QString StrProcessName,
	QString StrProcessPara,
	ProcessType &m_PType,
	ImageProcess **pImageProcess
)
{
	// 参数声明
	int iRet;
	
	// 处理类型判断
	if ("GrayTrans" == StrProcessName)
	{
		m_PType = ProcessType_GrayTrans;
	}
	else if ("Enhancement" == StrProcessName)
	{
		m_PType = ProcessType_Enhancement;
	}
	else
	{
		return ERR_INPUT;
	}
	
	// 解析参数
	switch (m_PType)
	{
		case ProcessType_GrayTrans:
		{
			// 初始化
			GrayTransProcess *pGrayTransPro = new GrayTransProcess;

			// 解析参数
			iRet = ParseGrayTransPara(StrProcessPara, pGrayTransPro);
			if (SUCCESS != iRet) return iRet;

			// 指针赋值
			*pImageProcess = pGrayTransPro;
			
			break;
		}
		default:
		{
			break;
		}		
	}


	return SUCCESS;
}

/**************************************************************
函数名： GetMarcoDBParaString_GrayTrans

定义 ：  获取用于宏数据库存储的参数字符（灰度变换）

入口参数 ：
	QString &StrPara,
	float fBottom,
	float fTop,
	float fR

函数返回值 ：

****************************************************************/
void Marco::GetMarcoDBParaString_GrayTrans
(
	QString &StrPara,
	float fBottom,
	float fTop,
	float fR
)
{
	// 初始化
	StrPara.clear();

	// 获取文件对话框
	StrPara = "[" + QString::number((double)fBottom, 'g', 3) + "]"
		+ "[" + QString::number((double)fTop, 'g', 3) + "]"
		+ "[" + QString::number((double)fR, 'g', 3) + "]";

	return;
}

/**************************************************************
函数名： ParseGrayTransPara

定义 ：  解码灰度变换参数

入口参数 ：
	QString StrPara,
	GrayTransProcess *pGrayTransPro

函数返回值 ：
	iRet :  0      -- 成功
			others -- 错误代码
****************************************************************/
int Marco::ParseGrayTransPara
(
	QString StrPara,
	GrayTransProcess *pGrayTransPro
)
{
	// 参数声明
	int iFrontPos;
	int iBackPos;
	QString StrNum;

	// Bottom值
	iFrontPos = StrPara.indexOf('[');
	iBackPos = StrPara.indexOf(']');
	StrNum = StrPara.mid(iFrontPos + 1, iBackPos - iFrontPos - 1);
	pGrayTransPro->SetBottom(StrNum.toFloat());

	// Top值
	iFrontPos = StrPara.indexOf('[', iBackPos);
	iBackPos = StrPara.indexOf(']', iFrontPos);
	StrNum = StrPara.mid(iFrontPos + 1, iBackPos - iFrontPos - 1);
	pGrayTransPro->SetTop(StrNum.toFloat());

	// R值
	iFrontPos = StrPara.indexOf('[', iBackPos);
	iBackPos = StrPara.indexOf(']', iFrontPos);
	StrNum = StrPara.mid(iFrontPos + 1, iBackPos - iFrontPos - 1);
	pGrayTransPro->SetR(StrNum.toFloat());

	return SUCCESS;
}

/**************************************************************
函数名： GetMarcoLabel

定义 ：  获取宏处理标识

入口参数 ：
	int iMarcoNo ：		宏编号

函数返回值 ：
	QString StrLabel :  宏标识
****************************************************************/
QString Marco::GetMarcoLabel(int iMarcoNo)
{	
	return m_MapMarco.KeyAt(iMarcoNo);
}

/**************************************************************
函数名： GetMarcoSize

定义 ：  获取宏长度

入口参数 ：
	
函数返回值 ：
	int iSize :  宏处理长度
****************************************************************/
int Marco::GetMarcoSize()
{
	return m_MapMarco.Size();
}

/**************************************************************
函数名： IsFastBtn

定义 ：  当前宏是否存在快速按钮

入口参数 ：
	int iMarcoNo ：宏编号

函数返回值 ：
	bool isBtn ： true  -- 存在
	              false -- 不存在 
****************************************************************/
bool Marco::IsFastBtn(int iMarcoNo)
{
	return m_MapMarco.IsBtn(iMarcoNo);
}

/**************************************************************
函数名： IsFastBtn

定义 ：  当前宏是否存在快速按钮

入口参数 ：
	QString StrMarcoLabel ：宏处理标识

函数返回值 ：
	bool isBtn ： true  -- 存在
				  false -- 不存在
****************************************************************/
bool Marco::IsFastBtn(QString StrMarcoLabel)
{
	return m_MapMarco.IsBtn(StrMarcoLabel);
}

/**************************************************************
函数名： SetFastBtn

定义 ：  设置快速按钮

入口参数 ：
	QString StrMarcoLabel ：  宏处理标识

函数返回值 ：

****************************************************************/
void Marco::SetFastBtn
(
	QString StrMarcoLabel, 
	bool bIsFastBtn
)
{
	// 设置快速按钮
	m_MapMarco.setBtn(StrMarcoLabel, bIsFastBtn);

	// 更新数据库
	UpdateDB();
}

/**************************************************************
函数名： GetImageProPipe

定义 ：  获取图像处理管道（宏处理）

入口参数 ：
	QString StrMarcoLabel ：		处理宏标识

函数返回值 ：
	QSharedPointer<ImageProPipe> ：	图像宏处理管道指针	  
****************************************************************/
QSharedPointer<ImageProPipe> Marco::GetImageProPipe(QString StrMarcoLabel)
{
	return m_MapMarco.Find(StrMarcoLabel);
}

/**************************************************************
函数名： DeleteMarco

定义 ：  删除宏处理

入口参数 ：
	QString StrMarcoLabel ：		处理宏标识

函数返回值 ：

****************************************************************/
void Marco::DeleteMarco(QString StrMarcoLabel)
{
	// 参数声明
	int iMarcoSize;

	// 宏Map长度判断
	iMarcoSize = m_MapMarco.Size();
	if (0 == iMarcoSize) return;

	// 删除Map中的宏处理
	m_MapMarco.DeleteProPipe(StrMarcoLabel);

	// 更新数据库
	UpdateDB();

}









/**************************************************************
函数名： Size

定义 ：  获取Map长度

入口参数 ：

函数返回值 ：
	int size ：Map长度值
****************************************************************/
int MapMarco::Size()
{
	return (int)SortList.size();
}

/**************************************************************
函数名： Insert

定义 ：  将图像处理管道插入Map

入口参数 ：
	QString StrLabel ：
	QSharedPointer<ImageProPipe> pInProPipe ：

函数返回值 ：
	int Err ： 0      -- 完成
			   others -- 未完成
****************************************************************/
void MapMarco::Insert
(
	QString StrLabel,
	QSharedPointer<ImageProPipe> pInProPipe
)
{
	// 参数声明
	QSharedPointer<ImageProPipe> pProPipe;
	QMap<QString, QSharedPointer<ImageProPipe>>::const_iterator it;    // QMap的迭代器

	// 检查是否有重复的图像实例
	it = MapProPipe.find(StrLabel);
	if (MapProPipe.end() != it) return;

	// 处理管道赋值
	pInProPipe.get()->Copy(pProPipe);

	// 载入图像实例
	MapProPipe.insert(StrLabel, pProPipe);

	// 载入图像标识队列
	SortList.insert(0, StrLabel);

	// 载入是否存在快捷按钮
	MapIsBtn.insert(StrLabel, false);
}

/**************************************************************
函数名： KeyAt

定义 ：  依据排序编号获取Key值

入口参数 ：
	int iNo ：        编号

函数返回值 ：
	QString StrKey ： Key值
****************************************************************/
QString MapMarco::KeyAt(int iNo)
{
	// 参数声明
	QString StrKey;

	// 初始化
	StrKey.clear();

	// 编号判断
	if ((iNo < 0) || (iNo >= SortList.size())) return StrKey;

	// Key值赋值
	StrKey = SortList[iNo];

	return StrKey;
}

/**************************************************************
函数名： ValueAt

定义 ：  依据排序编号获取Value值

入口参数 ：
	int iNo ：        编号

函数返回值 ：
	QSharedPointer<ImageProPipe>  ： Value值
****************************************************************/
QSharedPointer<ImageProPipe> MapMarco::ValueAt(int iNo)
{
	// 参数声明
	QString StrKey;
	QSharedPointer<ImageProPipe> pProPipe;
	QMap<QString, QSharedPointer<ImageProPipe>>::iterator it;

	// 初始化
	pProPipe.clear();

	// 编号判断
	if ((iNo < 0) || (iNo >= SortList.size())) return pProPipe;

	// key值获取
	StrKey = SortList[iNo];

	// 迭代指针获取
	it = MapProPipe.find(StrKey);

	return it.value();
}

/**************************************************************
函数名： Find

定义 ：  依据key值寻找value值

入口参数 ：
	int iNo ：        编号

函数返回值 ：
	QSharedPointer<ImageInstance>  ： Value值
****************************************************************/
QSharedPointer<ImageProPipe> MapMarco::Find(QString StrKey)
{
	// 参数声明
	QSharedPointer<ImageProPipe> pProPipe;
	QMap<QString, QSharedPointer<ImageProPipe>>::iterator it;

	// 初始化
	pProPipe.clear();

	// 迭代指针获取
	it = MapProPipe.find(StrKey);
	if (it == MapProPipe.cend()) return pProPipe;

	return it.value();
}

/**************************************************************
函数名： Clear

定义 ：  清除

入口参数 ：

函数返回值 ：
	
****************************************************************/
void MapMarco::Clear()
{
	MapProPipe.clear();
	SortList.clear();
	MapIsBtn.clear();

	return;
}

/**************************************************************
函数名： setBtn

定义 ：  设置是否存在快捷按钮

入口参数 ：
	QString StrLabel ：	处理管道标识
	bool bIsBtn ：		是否存在快捷按钮

函数返回值 ：

****************************************************************/
void MapMarco::setBtn
(
	QString StrLabel,
	bool bIsBtn
)
{
	// 参数声明
	QMap<QString, bool>::iterator it;

	// 迭代指针获取
	it = MapIsBtn.find(StrLabel);
	if (it == MapIsBtn.cend()) return;
	
	// 赋值
	it.value() = bIsBtn;

	return;
}

/**************************************************************
函数名： IsBtn

定义 ：  查询是否存在快捷按钮

入口参数 ：
	QString StrLabel ：	处理管道标识

函数返回值 ：

****************************************************************/
bool MapMarco::IsBtn(QString StrLabel)
{
	// 参数声明
	QMap<QString, bool>::iterator it;

	// 迭代指针获取
	it = MapIsBtn.find(StrLabel);
	if (it == MapIsBtn.cend()) return false;

	// 赋值
	return it.value();
}

/**************************************************************
函数名： IsBtn

定义 ：  查询是否存在快捷按钮

入口参数 ：
	QString StrLabel ：	处理管道标识

函数返回值 ：
	bool isBtn ： true  -- 存在
	              false -- 不存在 
****************************************************************/
bool MapMarco::IsBtn(int iNo)
{
	// 参数声明
	QString StrKey;
	QMap<QString, bool>::iterator it;

	// 编号判断
	if ((iNo < 0) || (iNo >= SortList.size())) return false;

	// key值获取
	StrKey = SortList[iNo];

	// 迭代指针获取
	it = MapIsBtn.find(StrKey);
	
	return it.value();
}
 
/**************************************************************
函数名： DeleteProPipe

定义 ：  删除指定的处理管道

入口参数 ：
	QString StrLabel ：	处理管道标识

函数返回值 ：
	
****************************************************************/
void MapMarco::DeleteProPipe(QString StrLabel)
{
	// 参数声明
	bool bRet;
	QMap<QString, QSharedPointer<ImageProPipe>>::iterator itImgProPipe;
	QMap<QString, bool>::iterator itIsBtn;
	
	// 删除排序队列中的宏信息
	bRet = SortList.removeOne(StrLabel);
	if (false == bRet) return;

	// 删除图像处理管道Map中的宏信息
	itImgProPipe = MapProPipe.find(StrLabel);
	if (itImgProPipe == MapProPipe.end()) return;

	// 删除快速按钮Map中的宏信息
	itIsBtn = MapIsBtn.find(StrLabel);
	if (itIsBtn == MapIsBtn.end()) return;
	
	return;
}
