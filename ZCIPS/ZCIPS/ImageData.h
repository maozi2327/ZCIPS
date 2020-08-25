#pragma once

#include <QSharedPointer>

/******** ZCIPSData 数据类 **********/
class ImageData
{
public:
	int iHeight;
	int iWidth;
	QSharedPointer<float> pfOrgData;		  // 载入的原始数据
	QSharedPointer<float> pfCurrData;		  // 当前处理步骤后的数据
	QSharedPointer<float> pfProcessingData;   // 处理中的数据

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

