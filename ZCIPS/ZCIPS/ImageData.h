#pragma once

#include <QSharedPointer>

/******** ZCIPSData ������ **********/
class ImageData
{
public:
	int iHeight;
	int iWidth;
	QSharedPointer<float> pfOrgData;		  // �����ԭʼ����
	QSharedPointer<float> pfCurrData;		  // ��ǰ������������
	QSharedPointer<float> pfProcessingData;   // �����е�����

public:
	ImageData();

public:
	bool InitData
	(
		int iInHeight,
		int iInWidth
	);

	void ClearData();

	bool GetDataFromFile(QString StrFileName);

	int ProcessingAccept();

	int ProcessingCancel();

	int ResetData();

	bool Copy(QSharedPointer<ImageData>& pCpImageData);
};

