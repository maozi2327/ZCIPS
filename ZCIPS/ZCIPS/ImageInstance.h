#pragma once

#include "ImageData.h"
#include "ImageProcess.h"
#include "ImageMark.h"

class ImageInstance
{

// ����
public:
	QSharedPointer<ImageProcessInstance> pImageProcessIns;      // ͼ����ʵ��
																// ������
																// 1 -- ͼ����ܵ�
																// 2 -- ��ǰ����

	// QSharedPointer<Marker> pMarker;

	QSharedPointer<ImageData> pImageData;						// ͼ������
	QSharedPointer<ImageMark>pImageMarks;                       //ͼ����

	ImageMouseType mouseType;                                   //�жϵ�ǰ����Ƿ�������Լ������ֱ��

// ��Ӧ������
public:
	ImageInstance();

// ����
public:	
	// ���ļ��м���ͼ��ʵ��
	int LoadFromFile(QString StrInPath);

	// ����ǰ������봦��ܵ�
	int AddProcessPipe();

	// ��ǰͼ�����ý��ܣ����ݷ����ı�
	int DataProccessAccept();

	// ��ǰͼ�����Ƿ����
	bool IsCurrProcessValid();

	// ��ǰͼ����ܵ��Ƿ����
	bool IsProcessPipeValid();

	// ����
	bool Copy(QSharedPointer<ImageInstance>& pImageIns);

	// �����µ�ͼ����ܵ�
	int LoadImageProPipe(QSharedPointer<ImageProPipe> pProcessPipe);

	// ����ܵ�ִ��
	int ProPipeExcute();

	// ����ܵ���ȡ
	int ProPipeCut(int iCutProNo);

	// ����ܵ����Ȼ�ȡ
	int ProPipeSize();

	// �����ǰ����
	void ClearCurrProcess();

	// �������ܵ�
	void ClearProPipe();

	// ��������
	void ResetImgData();
};


class ImageInstanceMap
{
public:

	QMap<QString, QSharedPointer<ImageInstance>> MapImageIns;	// ͼ��ʵ��Map
															    // QString -- ͼ���ʶ
															    // ImageInstance -- ͼ��ʵ��

	QList<QString> SortList;                                    // ��ΪMap�޷�����
	                                                            // ����һ��List����������

	

public:

	// ��ȡMap�ĳ���
	int Size();

	// ��ͼ��ʵ������Map
	void Insert(QString StrLabel, QSharedPointer<ImageInstance> pImageIns);

	//��ͼ��ʵ����Map��ɾ��
	void Delete(QString StrLabel);

	// ��ȡ��Ӧ��ŵ�Keyֵ
	QString KeyAt(int iNo);

	// ��ȡ��Ӧ��ŵ�Valueֵ
	QSharedPointer<ImageInstance> ValueAt(int iNo);

	// Valueֵ��Ѱ
	QSharedPointer<ImageInstance> Find(QString StrKey);
};