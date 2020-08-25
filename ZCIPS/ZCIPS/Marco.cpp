#include "Marco.h"
#include "ERRInfo.h"

/**************************************************************
�������� Marco

���� ��  ���캯��

��ڲ��� ��

��������ֵ ��

****************************************************************/
Marco::Marco()
{
	// �������ݿ�
	bIsConnected = ConnectDB();
	if (false == bIsConnected) return;
	
	// �����
	UpdateMarcoFromDB();

}

/**************************************************************
�������� ConnectDB

���� ��  �������ݿ�

��ڲ��� ��

��������ֵ ��
	true ��  �ɹ�
	false �� ʧ��
****************************************************************/
bool Marco::ConnectDB()
{
	// ��������
	bool bRet;
	bool bIsExsit;
	QString StrDataBasePath;
	QSqlQuery m_Query;           // ��ѯʵ��
	
	//�������ݿ�����
	m_Database = QSqlDatabase::addDatabase("QSQLITE");

	//�������ݿ�����
	// ��ȡ���ݿ��ļ�·��
	StrDataBasePath = QCoreApplication::applicationDirPath();
	StrDataBasePath = StrDataBasePath + "\\Marco.db3";
	m_Database.setDatabaseName(StrDataBasePath);

	//�����ݿ�
	bRet = m_Database.open();
	if (false == bRet) return false;

	// ��ѯʵ����ʼ��
	m_Query = QSqlQuery(m_Database);

	// �жϱ���Ƿ����
	// �������򴴽����
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
�������� DeleteDB

���� ��  ɾ�����ݿ�

��ڲ��� ��

��������ֵ ��
	
****************************************************************/
void Marco::DeleteDB()
{
	// ��������
	QString StrMarcoLabel;
	QString QuerySentence;
	QSqlQuery m_Query;
	QSqlQuery m_SubQuery;
	
	// ���Ӽ��
	if (false == bIsConnected) return;
	
	// ��Query�����ݿ�����
	m_Query = QSqlQuery(m_Database);
	m_SubQuery = QSqlQuery(m_Database);

	// ��ȡ������MarcoTable
	m_Query.exec("SELECT * FROM MarcoTable");
	while (m_Query.next())
	{
		// ��ȡ���ʶ��ͬʱҲ��ȡ�˺��ӱ���
		StrMarcoLabel = m_Query.value(1).toString();

		// ɾ����Ӧ�ĺ��ӱ�
		QuerySentence.clear();
		QuerySentence = "DROP TABLE " + StrMarcoLabel;
		m_SubQuery.exec(QuerySentence);
	}
	
	// ��պ�����
	m_Query.exec("DELETE FROM MarcoTable");  // ɾ�����
	m_Query.exec("VACCUUM");				 // �ͷ��ڴ�ռ�

	return;
}

/**************************************************************
�������� UpdateDB

���� ��  ���ݺ�������ݿ�

��ڲ��� ��

��������ֵ ��
	
****************************************************************/
void Marco::UpdateDB()
{
	// ��������
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

	// ���Ӽ��
	if (false == bIsConnected) return;

	// ����
	if (0 == m_MapMarco.Size()) return;

	// ɾ�����
	DeleteDB();

	// ��Query�����ݿ�����
	m_Query = QSqlQuery(m_Database);

	// Marco -> DB
	for (int iKeyNo = 0; iKeyNo < m_MapMarco.Size(); iKeyNo++)
	{
		// ��ȡ���ʶ�봦��
		MarcoLabel = m_MapMarco.KeyAt(iKeyNo);
		pImageProPipe = m_MapMarco.ValueAt(iKeyNo);
		if (true == m_MapMarco.IsBtn(iKeyNo)) iBool = 1;
		else iBool = 0;
		
		// ��д������
		m_Query.prepare("INSERT INTO MarcoTable (ID, MarcoName, FastButton)"
			"VALUES (:ID, :MarcoName, :FastButton)");
		m_Query.bindValue(":ID", (iKeyNo + 1));
		m_Query.bindValue(":MarcoName", MarcoLabel);
		m_Query.bindValue(":FastButton", iBool);
		m_Query.exec();

		// �������ӱ�
		StrSentence.clear();
		StrSentence = "CREATE TABLE "
			+ MarcoLabel
			+ " ( ID INT PRIMARY KEY, "
			+ "ProcessName nvarchar(100), "
			+ "ProcessPara nvarchar(200))";
		m_Query.exec(StrSentence);

		// ��д���ӱ�
		for (int iProNo = 0; iProNo < pImageProPipe.get()->GetPipeLen(); iProNo++)
		{
			// ��ȡ����ʽ
			iRet = pImageProPipe.get()->PipeGet(iProNo, InType, &pImageProcess);
			if (SUCCESS != iRet) return;
			
			// 
			switch (InType)
			{
				// �޴���
				case ProcessType_None: return;

				// �Ҷȱ任
				case ProcessType_GrayTrans:
				{
					// ��ȡ������
					StrProcessName.clear();
					StrProcessName = "GrayTrans";
					
					// ��ȡ�������
					GrayTransProcess *pGrayTransPro
						= (GrayTransProcess *)pImageProcess;
					StrProcessPara.clear();
					GetMarcoDBParaString_GrayTrans(StrProcessPara,
						pGrayTransPro->GetBottom(),
						pGrayTransPro->GetTop(),
						pGrayTransPro->GetR());

					break;
				}

				// ͼ����ǿ
				case ProcessType_Enhancement:
				{
					break;
				}

				default:
				{
					return;
				}
			}

			// ��д���ӱ�
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
�������� UpdateMarcoFromDB

���� ��  �������ݿ���º�

��ڲ��� ��

��������ֵ ��

****************************************************************/
void Marco::UpdateMarcoFromDB()
{
	// ��������
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

	// ���Ӽ��
	if (false == bIsConnected) return;
	
	// �����
	DeleteMarcoMap();

	// ��Query�����ݿ�����
	m_Query = QSqlQuery(m_Database);
	m_SubQuery = QSqlQuery(m_Database);

	// ��ȡ�����
	m_Query.exec(QObject::tr("select * from MarcoTable"));
	while (m_Query.next())
	{
		// ��ȡ���ʶ��ͬʱҲ��ȡ�˺��ӱ���
		StrMarcoLabel = m_Query.value(1).toString();

		// ��ʼ������ܵ�
		pProPipe.reset(new ImageProPipe);

		// ��ȡ�ӱ��
		StrQuerySentence = "SELECT * FROM " + StrMarcoLabel;
		m_SubQuery.exec(StrQuerySentence);
		while (m_SubQuery.next())
		{
			// ��ȡ������
			StrProcessName.clear();
			StrProcessName = m_SubQuery.value(1).toString();

			// ��ȡ�������
			StrProcessPara.clear();
			StrProcessPara = m_SubQuery.value(2).toString();

			// ����ͼ����
			iRet = ParseProcess(StrProcessName, StrProcessPara, m_PType, &pImageProcess);
			if (SUCCESS != iRet) return;

			// ������ͼ����ܵ�
			iRet = pProPipe.get()->PipeAppend(m_PType, pImageProcess);
			if (SUCCESS != iRet) return;
		}

		// ���ؽ����Map
		m_MapMarco.Insert(StrMarcoLabel, pProPipe);

		// ��ȡ�Ƿ���ڿ��ٰ�ť
		StrIsFastBtn = m_Query.value(2).toString();
		if ("1" == StrIsFastBtn) 
			m_MapMarco.setBtn(StrMarcoLabel, true);
		else m_MapMarco.setBtn(StrMarcoLabel, false);
	}
}

/**************************************************************
�������� InsertProPipe

���� ��  ���봦��ܵ�����

��ڲ��� ��
	QString StrLabel ����ʶ
	QSharedPointer<ImageProPipe> pImgProPipe �� 

��������ֵ ��

****************************************************************/
void Marco::InsertProPipe
(
	QString StrLabel,
	QSharedPointer<ImageProPipe> pImgProPipe
)
{
	// ������ܵ��������
	m_MapMarco.Insert(StrLabel, pImgProPipe);

	// �������ݿ�
	UpdateDB();
	
	return;
}

/**************************************************************
�������� DeleteMarcoMap

���� ��  ɾ����

��ڲ��� ��

��������ֵ ��

****************************************************************/
void Marco::DeleteMarcoMap()
{
	// ɾ�����к�
	m_MapMarco.Clear();

	return;
}

/**************************************************************
�������� isConnectDataBase

���� ��  �Ƿ����ӳɹ�

��ڲ��� ��

��������ֵ ��
	true ��  �ɹ�
	false �� ʧ��
****************************************************************/
bool Marco::isConnectDataBase(){ return bIsConnected; }

/**************************************************************
�������� isConnectDataBase

���� ��  �Ƿ����ӳɹ�

��ڲ��� ��
	QString StrProcessName ��		
	QString StrProcessPara ��
	ProcessType &m_PType ��
	ImageProcess **pImageProcess ��

��������ֵ ��
	iRet :  0      -- �ɹ�
	        others -- �������
****************************************************************/
int Marco::ParseProcess
(
	QString StrProcessName,
	QString StrProcessPara,
	ProcessType &m_PType,
	ImageProcess **pImageProcess
)
{
	// ��������
	int iRet;
	
	// ���������ж�
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
	
	// ��������
	switch (m_PType)
	{
		case ProcessType_GrayTrans:
		{
			// ��ʼ��
			GrayTransProcess *pGrayTransPro = new GrayTransProcess;

			// ��������
			iRet = ParseGrayTransPara(StrProcessPara, pGrayTransPro);
			if (SUCCESS != iRet) return iRet;

			// ָ�븳ֵ
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
�������� GetMarcoDBParaString_GrayTrans

���� ��  ��ȡ���ں����ݿ�洢�Ĳ����ַ����Ҷȱ任��

��ڲ��� ��
	QString &StrPara,
	float fBottom,
	float fTop,
	float fR

��������ֵ ��

****************************************************************/
void Marco::GetMarcoDBParaString_GrayTrans
(
	QString &StrPara,
	float fBottom,
	float fTop,
	float fR
)
{
	// ��ʼ��
	StrPara.clear();

	// ��ȡ�ļ��Ի���
	StrPara = "[" + QString::number((double)fBottom, 'g', 3) + "]"
		+ "[" + QString::number((double)fTop, 'g', 3) + "]"
		+ "[" + QString::number((double)fR, 'g', 3) + "]";

	return;
}

/**************************************************************
�������� ParseGrayTransPara

���� ��  ����Ҷȱ任����

��ڲ��� ��
	QString StrPara,
	GrayTransProcess *pGrayTransPro

��������ֵ ��
	iRet :  0      -- �ɹ�
			others -- �������
****************************************************************/
int Marco::ParseGrayTransPara
(
	QString StrPara,
	GrayTransProcess *pGrayTransPro
)
{
	// ��������
	int iFrontPos;
	int iBackPos;
	QString StrNum;

	// Bottomֵ
	iFrontPos = StrPara.indexOf('[');
	iBackPos = StrPara.indexOf(']');
	StrNum = StrPara.mid(iFrontPos + 1, iBackPos - iFrontPos - 1);
	pGrayTransPro->SetBottom(StrNum.toFloat());

	// Topֵ
	iFrontPos = StrPara.indexOf('[', iBackPos);
	iBackPos = StrPara.indexOf(']', iFrontPos);
	StrNum = StrPara.mid(iFrontPos + 1, iBackPos - iFrontPos - 1);
	pGrayTransPro->SetTop(StrNum.toFloat());

	// Rֵ
	iFrontPos = StrPara.indexOf('[', iBackPos);
	iBackPos = StrPara.indexOf(']', iFrontPos);
	StrNum = StrPara.mid(iFrontPos + 1, iBackPos - iFrontPos - 1);
	pGrayTransPro->SetR(StrNum.toFloat());

	return SUCCESS;
}

/**************************************************************
�������� GetMarcoLabel

���� ��  ��ȡ�괦���ʶ

��ڲ��� ��
	int iMarcoNo ��		����

��������ֵ ��
	QString StrLabel :  ���ʶ
****************************************************************/
QString Marco::GetMarcoLabel(int iMarcoNo)
{	
	return m_MapMarco.KeyAt(iMarcoNo);
}

/**************************************************************
�������� GetMarcoSize

���� ��  ��ȡ�곤��

��ڲ��� ��
	
��������ֵ ��
	int iSize :  �괦����
****************************************************************/
int Marco::GetMarcoSize()
{
	return m_MapMarco.Size();
}

/**************************************************************
�������� IsFastBtn

���� ��  ��ǰ���Ƿ���ڿ��ٰ�ť

��ڲ��� ��
	int iMarcoNo ������

��������ֵ ��
	bool isBtn �� true  -- ����
	              false -- ������ 
****************************************************************/
bool Marco::IsFastBtn(int iMarcoNo)
{
	return m_MapMarco.IsBtn(iMarcoNo);
}

/**************************************************************
�������� IsFastBtn

���� ��  ��ǰ���Ƿ���ڿ��ٰ�ť

��ڲ��� ��
	QString StrMarcoLabel ���괦���ʶ

��������ֵ ��
	bool isBtn �� true  -- ����
				  false -- ������
****************************************************************/
bool Marco::IsFastBtn(QString StrMarcoLabel)
{
	return m_MapMarco.IsBtn(StrMarcoLabel);
}

/**************************************************************
�������� SetFastBtn

���� ��  ���ÿ��ٰ�ť

��ڲ��� ��
	QString StrMarcoLabel ��  �괦���ʶ

��������ֵ ��

****************************************************************/
void Marco::SetFastBtn
(
	QString StrMarcoLabel, 
	bool bIsFastBtn
)
{
	// ���ÿ��ٰ�ť
	m_MapMarco.setBtn(StrMarcoLabel, bIsFastBtn);

	// �������ݿ�
	UpdateDB();
}

/**************************************************************
�������� GetImageProPipe

���� ��  ��ȡͼ����ܵ����괦��

��ڲ��� ��
	QString StrMarcoLabel ��		������ʶ

��������ֵ ��
	QSharedPointer<ImageProPipe> ��	ͼ��괦��ܵ�ָ��	  
****************************************************************/
QSharedPointer<ImageProPipe> Marco::GetImageProPipe(QString StrMarcoLabel)
{
	return m_MapMarco.Find(StrMarcoLabel);
}

/**************************************************************
�������� DeleteMarco

���� ��  ɾ���괦��

��ڲ��� ��
	QString StrMarcoLabel ��		������ʶ

��������ֵ ��

****************************************************************/
void Marco::DeleteMarco(QString StrMarcoLabel)
{
	// ��������
	int iMarcoSize;

	// ��Map�����ж�
	iMarcoSize = m_MapMarco.Size();
	if (0 == iMarcoSize) return;

	// ɾ��Map�еĺ괦��
	m_MapMarco.DeleteProPipe(StrMarcoLabel);

	// �������ݿ�
	UpdateDB();

}









/**************************************************************
�������� Size

���� ��  ��ȡMap����

��ڲ��� ��

��������ֵ ��
	int size ��Map����ֵ
****************************************************************/
int MapMarco::Size()
{
	return (int)SortList.size();
}

/**************************************************************
�������� Insert

���� ��  ��ͼ����ܵ�����Map

��ڲ��� ��
	QString StrLabel ��
	QSharedPointer<ImageProPipe> pInProPipe ��

��������ֵ ��
	int Err �� 0      -- ���
			   others -- δ���
****************************************************************/
void MapMarco::Insert
(
	QString StrLabel,
	QSharedPointer<ImageProPipe> pInProPipe
)
{
	// ��������
	QSharedPointer<ImageProPipe> pProPipe;
	QMap<QString, QSharedPointer<ImageProPipe>>::const_iterator it;    // QMap�ĵ�����

	// ����Ƿ����ظ���ͼ��ʵ��
	it = MapProPipe.find(StrLabel);
	if (MapProPipe.end() != it) return;

	// ����ܵ���ֵ
	pInProPipe.get()->Copy(pProPipe);

	// ����ͼ��ʵ��
	MapProPipe.insert(StrLabel, pProPipe);

	// ����ͼ���ʶ����
	SortList.insert(0, StrLabel);

	// �����Ƿ���ڿ�ݰ�ť
	MapIsBtn.insert(StrLabel, false);
}

/**************************************************************
�������� KeyAt

���� ��  ���������Ż�ȡKeyֵ

��ڲ��� ��
	int iNo ��        ���

��������ֵ ��
	QString StrKey �� Keyֵ
****************************************************************/
QString MapMarco::KeyAt(int iNo)
{
	// ��������
	QString StrKey;

	// ��ʼ��
	StrKey.clear();

	// ����ж�
	if ((iNo < 0) || (iNo >= SortList.size())) return StrKey;

	// Keyֵ��ֵ
	StrKey = SortList[iNo];

	return StrKey;
}

/**************************************************************
�������� ValueAt

���� ��  ���������Ż�ȡValueֵ

��ڲ��� ��
	int iNo ��        ���

��������ֵ ��
	QSharedPointer<ImageProPipe>  �� Valueֵ
****************************************************************/
QSharedPointer<ImageProPipe> MapMarco::ValueAt(int iNo)
{
	// ��������
	QString StrKey;
	QSharedPointer<ImageProPipe> pProPipe;
	QMap<QString, QSharedPointer<ImageProPipe>>::iterator it;

	// ��ʼ��
	pProPipe.clear();

	// ����ж�
	if ((iNo < 0) || (iNo >= SortList.size())) return pProPipe;

	// keyֵ��ȡ
	StrKey = SortList[iNo];

	// ����ָ���ȡ
	it = MapProPipe.find(StrKey);

	return it.value();
}

/**************************************************************
�������� Find

���� ��  ����keyֵѰ��valueֵ

��ڲ��� ��
	int iNo ��        ���

��������ֵ ��
	QSharedPointer<ImageInstance>  �� Valueֵ
****************************************************************/
QSharedPointer<ImageProPipe> MapMarco::Find(QString StrKey)
{
	// ��������
	QSharedPointer<ImageProPipe> pProPipe;
	QMap<QString, QSharedPointer<ImageProPipe>>::iterator it;

	// ��ʼ��
	pProPipe.clear();

	// ����ָ���ȡ
	it = MapProPipe.find(StrKey);
	if (it == MapProPipe.cend()) return pProPipe;

	return it.value();
}

/**************************************************************
�������� Clear

���� ��  ���

��ڲ��� ��

��������ֵ ��
	
****************************************************************/
void MapMarco::Clear()
{
	MapProPipe.clear();
	SortList.clear();
	MapIsBtn.clear();

	return;
}

/**************************************************************
�������� setBtn

���� ��  �����Ƿ���ڿ�ݰ�ť

��ڲ��� ��
	QString StrLabel ��	����ܵ���ʶ
	bool bIsBtn ��		�Ƿ���ڿ�ݰ�ť

��������ֵ ��

****************************************************************/
void MapMarco::setBtn
(
	QString StrLabel,
	bool bIsBtn
)
{
	// ��������
	QMap<QString, bool>::iterator it;

	// ����ָ���ȡ
	it = MapIsBtn.find(StrLabel);
	if (it == MapIsBtn.cend()) return;
	
	// ��ֵ
	it.value() = bIsBtn;

	return;
}

/**************************************************************
�������� IsBtn

���� ��  ��ѯ�Ƿ���ڿ�ݰ�ť

��ڲ��� ��
	QString StrLabel ��	����ܵ���ʶ

��������ֵ ��

****************************************************************/
bool MapMarco::IsBtn(QString StrLabel)
{
	// ��������
	QMap<QString, bool>::iterator it;

	// ����ָ���ȡ
	it = MapIsBtn.find(StrLabel);
	if (it == MapIsBtn.cend()) return false;

	// ��ֵ
	return it.value();
}

/**************************************************************
�������� IsBtn

���� ��  ��ѯ�Ƿ���ڿ�ݰ�ť

��ڲ��� ��
	QString StrLabel ��	����ܵ���ʶ

��������ֵ ��
	bool isBtn �� true  -- ����
	              false -- ������ 
****************************************************************/
bool MapMarco::IsBtn(int iNo)
{
	// ��������
	QString StrKey;
	QMap<QString, bool>::iterator it;

	// ����ж�
	if ((iNo < 0) || (iNo >= SortList.size())) return false;

	// keyֵ��ȡ
	StrKey = SortList[iNo];

	// ����ָ���ȡ
	it = MapIsBtn.find(StrKey);
	
	return it.value();
}
 
/**************************************************************
�������� DeleteProPipe

���� ��  ɾ��ָ���Ĵ���ܵ�

��ڲ��� ��
	QString StrLabel ��	����ܵ���ʶ

��������ֵ ��
	
****************************************************************/
void MapMarco::DeleteProPipe(QString StrLabel)
{
	// ��������
	bool bRet;
	QMap<QString, QSharedPointer<ImageProPipe>>::iterator itImgProPipe;
	QMap<QString, bool>::iterator itIsBtn;
	
	// ɾ����������еĺ���Ϣ
	bRet = SortList.removeOne(StrLabel);
	if (false == bRet) return;

	// ɾ��ͼ����ܵ�Map�еĺ���Ϣ
	itImgProPipe = MapProPipe.find(StrLabel);
	if (itImgProPipe == MapProPipe.end()) return;

	// ɾ�����ٰ�ťMap�еĺ���Ϣ
	itIsBtn = MapIsBtn.find(StrLabel);
	if (itIsBtn == MapIsBtn.end()) return;
	
	return;
}
