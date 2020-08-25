#pragma once

// Opencv��ĺ궨��
#ifdef _DEBUG
	#define OPENCV_LIB  "opencv_world410d.lib"
#else
	#define OPENCV_LIB  "opencv_world410.lib"
#endif

#include <QtWidgets>
#include <QSharedPointer>
#include "AreaSelect.h"

/***************************** ö������ ******************************/
enum ProcessType
{
	ProcessType_None = 0,
	ProcessType_GrayTrans,
	ProcessType_Enhancement
};


/******************************** �� **********************************/

// ͼ�������ࣩ
class ImageProcess
{
public:
	virtual int ImagePro(QSharedPointer<float> pfSrc,
		QSharedPointer<float> pfDes, int iWidth, int iHeight) = 0;

	virtual int GetParaString(QString &StrPara) = 0;
};

// �Ҷȱ任��
class GrayTransProcess : public ImageProcess
{

// ����
private:
	float fBottom;     // ���Ա任���� -- ����
	float fTop;        // ���Ա任���� -- ����
	float fR;          // ָ���任���� -- Rֵ
	QSharedPointer<AreaSelect> pAreaSelect;   // ͼ��������

// ����
public:
	// ���캯��
	GrayTransProcess();

	// ͼ����
	int ImagePro
	(QSharedPointer<float> pfSrc, QSharedPointer<float> pfDes,
	 int iWidth, int iHeight) override;
	
	// ��������
	void SetPara(float fInBottom, float fInTop, float fInR);

	// ��������
	void SetBottom(float fInBottom);

	// ��������
	void SetTop(float fInTop);

	// Rֵ����
	void SetR(float fInR);

	// ��ȡֵ
	float GetBottom();

	// ��ȡֵ
	float GetTop();

	// ��ȡֵ
	float GetR();

	// ���ô�������
	void setArea(QSharedPointer<AreaSelect> pInAreaSel);

	// ��ȡ�����ַ�
	int GetParaString(QString &StrPara);

	// ����
	bool Copy(GrayTransProcess** pCpProcess);

	// �ж��Ƿ��������ı�
	bool bIsValChanged();

};

// ͼ����ͨ��
class ImageProPipe
{
// ����
private:
	QList<QPair<ProcessType, ImageProcess*>> PipeList;

// ����
public:
	~ImageProPipe();

	// ��ȡ����
	int GetPipeLen();

	// ���ͼ����
	int PipeAppend(ProcessType InType, ImageProcess* pImageProcess);

	// ��ȡͼ������
	int PipeGet(int iNo, ProcessType &InType, ImageProcess **pImageProcess);

	// ����ܵ���ȡ
	int ProPipeCut(int iProCutNo);

	// ɾ������ʽ
	int DeletePro(int iProNo);

	// ���ƺ���
	bool Copy(QSharedPointer<ImageProPipe> &pCpPipe);

	// ɾ������ܵ�
	void DeletePipe();

	// ɾ������ʽָ��
	void DeleteProPointer(QPair<ProcessType, ImageProcess*> PairProcess);

};

// ͼ����ʵ����
class ImageProcessInstance
{


// ����
public:

	QSharedPointer<ImageProPipe> pProcessPipe;			 // ͼ����ͨ��

	QPair<ProcessType, ImageProcess*> PairCurrProcess;   // ��ǰͼ����

// ��Ӧ������
public:
	ImageProcessInstance();

// ����
public:
	bool Copy(QSharedPointer<ImageProcessInstance>& pCpImageProcessIns);

	int AddCurrProcessToPipe();

	bool IsCurrProcessValid();

	bool IsProcessPipeValid();

	void ClearCurrProcess();

	void ClearProPipe();

	int LoadImageProPipe(QSharedPointer<ImageProPipe> pInProcessPipe);

	int ProPipeExcute
	(
		QSharedPointer<float> pInData,
		QSharedPointer<float> pOutData,
		int iWidth,
		int iHeight
	);

	int ProPipeCut(int iCutProNo);
};