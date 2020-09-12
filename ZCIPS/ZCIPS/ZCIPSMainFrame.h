#pragma once

#include <QtWidgets>
#include "ui_ZCIPSMainFrame.h"
#include "ImageInstance.h"
#include "ImageDisplay.h"
#include "SubUI.h"
#include "AreaSelect.h"
#include "Marco.h"
#include "DataBaseProcess.h"
#include "../CTScan/ctscanexport.h"

/***************************** �� ******************************/



/***************************** ö������ ******************************/
enum DisplayMode
{
	DisplayMode_None = 0,	// ����ʾ
	DisplayMode_Detector,   // ̽����������ʾ
	DisplayMode_ImageList	// ͼ���б���ʾ
};

enum DetectorNo
{
	Detector_1 = 1,     // 1��̽����
	Detector_2          // 2��̽����
};

enum PanelTab
{
	PanelTab_Control = 0,		// ���ƽ���
	PanelTab_ImagePro			// ͼ�������
};

enum ImageProcessTab
{
	ImageProcessTab_GrayTrans = 0,		// �Ҷȱ任����
	ImageProcessTab_Enhancement			// ͼ����ǿ����
};

/***************************** �� ******************************/

class ZCIPSMainFrame : public QMainWindow
{
	Q_OBJECT

public:
	ZCIPSMainFrame(QWidget *parent = Q_NULLPTR);

private:
	Ui::ZCIPSMainFrameClass ui;
	
// ����
public:
	QPair<QString, QSharedPointer<ImageInstance>> PairDetIns_1;		// ��һ��̽������ͼ��ʵ��
	
	QPair<QString, QSharedPointer<ImageInstance>> PairDetIns_2;		// �ڶ���̽������ͼ��ʵ��
	
	ImageInstanceMap MapImageIns;									// ͼ��ʵ���б�
																	// QString -- ͼ���ʶ
																	// ImageInstance -- ͼ��ʵ��

	QPair<QString, QSharedPointer<ImageInstance>> PairCurrImageIns;   // ��ǰ��ʾ�����ͼ��ʵ��
																	  // QString -- ͼ���ʶ
																	  // ImageInstance -- ͼ��ʵ��
	
	QSharedPointer<ImageDisplay> pImageDisplay;		// ��ͼ��ʵ���е�ͼ����������
													// ת��Ϊ��ʾ��QWidget

	QSharedPointer<AreaSelect> pAreaSelect;			// ����ѡ�񣬸����������ͼ�������ѡ��
													// �����Ӧ����������
	
	QSharedPointer<Marco> pMarco;       // �괦��

	bool bIsUpdatedDisplay;				// �Ƿ����ͼ����ʾ��ʶλ��
										// �ñ�־λ���ڱ�������Ӧ�����������ͼ����¹���Ƶ��
	
	bool bIsUpdateImageIns;             // �Ƿ����ͼ��ʵ����ʶ
	                                    // ���������������嵼��ͼ��ʵ��������
	
	bool bIsUpdatePanel;                // �Ƿ����Panel�����ʶ
										// ���������������嵼��Panel���汻����
	
public slots:

	// ����ͼ��ť��Ӧ�ۺ���
	void LoadImageFromFileSlot();

	// PanelTab �л�����
	void SwitchPanelTab(int iIndex);

	// PanelTab �л�����
	void SwitchImageProcessTab(int iIndex);

// ����
public:

	// ��������ʼ��
	void InitialUI();

	// ����Panel
	void UpdatePanel();

	// ����ǰ��ͼ��������ͼ����ܵ�
	void CurrProccessToPipe();

	// �жϵ�ǰ��ʾ��ͼ��ʵ���Ƿ����
	bool IsCurrImageInstanceValid();

	// ��ǰ�����Ƿ���ڲ����ı�
	bool IsCurrProValChanged();

	// �жϵ�ǰ̽����ʵ���Ƿ����
	bool IsDetInstanceValid(DetectorNo DetNo);

	// ��ȡ��ǰ��ʾ��ͼ��ʵ��
	void GetCurrImageInstance();

/*********************** ͼ����ʾ���� Start ************************/
// �ؼ�
public:
	QSplitter *pScreenSplitter;   // ��Ļ�з�����
	QMdiArea *pScreenMdiArea;     // ������ʾ�򿪵�ͼ��

// ����
public:
	DetectorNo m_DetectorNo;
	DisplayMode m_DisplayMode;

// ����
public:

	// ��ʾ��ʼ��
	void InitialImgDisplay();

	// ��ʾģʽ�л�
	void DisplaySwitch();

	// ������ʾͼ������
	void UpdateImage();

	// ѡ��̽����
	void SeletctDet(DetectorNo DetNo);

// �ۺ���
public slots:
	
	// ͼ��ʵ������
	void MdiAreaSwitch(QMdiSubWindow* pSubWidget);
	//ͼ��ʵ��ɾ��
	void DeleteInstance(QString strWinTitle);

/*********************** ͼ����ʾ���� End ************************/

/*********************** �괦����� Start ************************/
public:
	MarcoUI m_MarcoUI;    // �괦�����

public:

	// �괦������ʼ��
	void MarcoUIInit();

	// ���º괦��ܵ���ʾ
	void MarcoTabUpdate();

	// ���´���ܵ���ʾ����
	void PipeDisUpdate();

	// ���ٴ���ť��ʾ����
	void FastBtnDisUpdate();

	// �ؼ�ʹ��
	void MarcoEnableCtrl();

// �ۺ���
public slots:

	// �����ɲۺ���
	void GenerateMarcoSlot(bool bChecked);

	// �����/�༭�ۺ���
	void LoadMarcoSlot(bool bChecked);

	// ��ȡ�ۺ���
	void CutSlot(bool bChecked);

	// ����ۺ���
	void ClearSlot(bool bChecked);

	// ִ�к����
	void MarcoExcute(QString StrMarcoLabel);

	// ִ�к�ɾ��
	void MarcoDelete(QString StrMarcoLabel);

	// �����ɿ��ٰ�ť
	void MarcoFastBtn(QString StrMarcoLabel);

/*********************** �괦����� End ************************/


/*********************** ͼ����Ҷȱ任 Start ************************/
public:
	GrayTransUI m_GrayTransUI;	// �Ҷȱ任����

public slots:

	// ���޻���ֵ�ı�
	void SliderLowerValChanged();

	// ���޻���ֵ�ı�
	void SliderMidValChanged();

	// ���޻���ֵ�ı�
	void SliderUpperValChanged();

	// ����Spinֵ���
	void SpinBottomValChanged(double dInVal);

	// ����Spinֵ���
	void SpinRValChanged(double dInVal);

	// ����Spinֵ���
	void SpinTopValChanged(double dInVal);

	// Ӧ�ð�ť�ۺ���
	void GrayTransOK();

	// ������ť�ۺ���
	void GrayTransCancel();

public:

	// �Ҷȱ任�����ʼ��
	void GrayTransUIInit();

	// �Ҷȱ任��ǩҳ����
	void GrayTransTabUpdate();

	// �Ҷȱ任��������
	void GrayTransTabReset();

	// ʹ�ܻ��ؼ�
	void EnableGrayTransCtrl(bool bEnable);

	// ͼ�����ݼ��ص�ֱ��ͼ
	void ImageDataToHistogram
	(
		QSharedPointer<float> pfData,
		int iWidth,
		int iHeight,
		QSharedPointer<AreaSelect> pArea
	);

	// ��Slider�ϵ����޻���ֵת��Ϊʵ�ʵ�Rֵ
	float TransR(int iBottom, int iMid, int iTop);

	// ��ʵ��Rֵת��ΪSlider�ϵ����޻���ֵ
	int VerTransR(int iBottom, int iTop, float fR);

/*********************** ͼ����Ҷȱ任 End ************************/
/*************************** ���ݿ⴦�� start ************************/
public:
	QSharedPointer<DataBaseProcess> DBProcess;    //��������ݿ���   
	void DataBaseUIInit();                        //���ݿ⴦����� 


public slots:
	void UpdateDBdisplyInfo(const QModelIndex index); //�������ݿ���ʾ��Ϣ
	void SearchDataBase();                            //��ѯ���ݿ�
	void UpdateDataBase();                            //�������ݿ� 
	void DeleteDataBaseEntry();                       //ɾ����¼
	void SubmmitDataBase();                           //�ύ�޸�
	void OpenSelectedFile(const QModelIndex index);

	/*************************** ���ݿ⴦�� end ************************/


/************************ ͼ���� start *************************/
public:
	//��ͼԪת��Ϊ�����Ϣ
	MarksBase*ItemToImageMark(QGraphicsItem*item);
	void GetActWinandCurrentIns();
public slots:
	void MarklistAdd(QGraphicsItem*item);
	void MarklistEdit(QGraphicsItem*item);
	//�л���ǰ������͵����α��
	void MarkChangeToRect();
	//�л���ǰ������͵���Բ���
	void MarkCHangeToEllipse();
	//�л���ǰ������͵��Զ�����״���
	void MarkChangeToPolygon();
	//�л���ǰ������͵����Ȳ���
	void MarkChangeToRuler();
	//ȡ�����ѡ������
	void MarkCancelMouseSelect();
	/******************************** ͼ���� end *******************/

private:
	CTScanInterface* ctScanHandle;
protected:
	void closeEvent(QCloseEvent *event) override;
};
