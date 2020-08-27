#include "ZCIPSMainFrame.h"
#include "ERRInfo.h"
#include "AddMarcoWidget.h"
#include "LoadMarcoWidget.h"
#include "ImageWidget.h"
#include "../CTScan/ctscanexport.h"
/**************************************************************
�������� ZCIPSMainFrame

���� ��  ���캯��

��ڲ��� ��
	QWidget *parent �� ��ָ��

��������ֵ ��

****************************************************************/
ZCIPSMainFrame::ZCIPSMainFrame(QWidget *parent)
	: QMainWindow(parent)
{
	// ������ʼ��
	bIsUpdatedDisplay = true;
	bIsUpdateImageIns = true;
	m_DisplayMode = DisplayMode_None;
	
	// ������ʵ����ʼ��
	pMarco.reset(new Marco);

	// �����ʼ��	
	ui.setupUi(this);   // ui��ʼ��
	InitialUI();        // ���������ڵ���

	// ��ʾ����
	DisplaySwitch();

	// �ۺ�������
	connect(ui.LoadFileAction, &QAction::triggered, this, &ZCIPSMainFrame::LoadImageFromFileSlot);
	connect(ui.MarkRectAction, &QAction::triggered, this, &ZCIPSMainFrame::MarkChangeToRect);
	connect(ui.MarkEllipseAction, &QAction::triggered, this, &ZCIPSMainFrame::MarkCHangeToEllipse);
	connect(ui.MarkPolyAction, &QAction::triggered, this, &ZCIPSMainFrame::MarkChangeToPolygon);
	//connect(ui.CancelSelectAction, &QAction::triggered, this, &ZCIPSMainFrame::MarkCancelMouseSelect);
	connect(ui.MarkRulerAction, &QAction::triggered, this, &ZCIPSMainFrame::MarkChangeToRuler);
}
/**************************************************************
�������� LoadImageSlot

���� ��  ����ͼ��ť��Ӧ�ۺ���

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::LoadImageFromFileSlot()
{
	// ��������
	int iRet;									// ��������ֵ
	int iHeight;								// ͼ��߶�
	int iWidth;									// ͼ����
	QSharedPointer<ImageInstance> pImageIns;    // ͼ��ʵ��
	QString StrFileName;						// �ļ�·��ȫ��
	QStringList StrlistFileName;				// �ļ���list
	QTextCodec* pTCode;							// �ַ����ͱ任ָ��
	
	// ��ȡ�ļ��Ի���
	pTCode = QTextCodec::codecForName("UTF-8");
	StrlistFileName = QFileDialog::getOpenFileNames
		(this, pTCode->toUnicode("��ͼ��"), 
		 "", tr("Image File(*.tif)"));
	if (true != StrlistFileName.isEmpty())
	{
		// ����ͼ��
		for (int Loop = 0; Loop < StrlistFileName.size(); Loop++)
		{
			// ͼ��ʵ������
			pImageIns.reset(new ImageInstance);
			
			// ͼ��ʵ������
			StrFileName = StrlistFileName[Loop];
			iRet = pImageIns.get()->LoadFromFile(StrFileName);
			if (SUCCESS != iRet) return;

			// ������ͼ��ʵ���б�
			MapImageIns.Insert(StrFileName, pImageIns);
		}
	}
	else return;

	// ��ʾģʽ���
	m_DisplayMode = DisplayMode_ImageList;

	// ͼ�����
	DisplaySwitch();

	// Panel����
	UpdatePanel();

}

/**************************************************************
�������� MdiAreaSwitch

���� ��  ͼ��ʵ������

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::MdiAreaSwitch(QMdiSubWindow* pSubWidget)
{
	// ��������
	int iHeight;
	int iWidth;

	// ����ָ��
	Q_UNUSED(pSubWidget);
	
	// ��ȡ��ǰ���ڴ����ͼ��ʵ��
	GetCurrImageInstance();
	
	// ������������
	pAreaSelect.clear();

	// ����Panel
	UpdatePanel();

	return;
}

/*****************************************************************/
//ͼ��ʵ��ɾ��
void ZCIPSMainFrame::DeleteInstance(QString strWinTitle)
{
	MapImageIns.Delete(strWinTitle);
}
/**************************************************************
�������� SwithPanelTab

���� ��  �л� Panel Tab

��ڲ��� ��
	int iIndex : Tab���

��������ֵ ��
	
****************************************************************/
void ZCIPSMainFrame::SwitchPanelTab(int iIndex)
{
	// ��������
	int iRet;
	PanelTab m_PanelTab;
	ImageProcessTab m_ImageProcessTab;

	// ����ǰͼ��������ͼ����ܵ�
	if (true == bIsUpdateImageIns)
	{
		CurrProccessToPipe();
	}
	
	// 
	m_PanelTab = (PanelTab)iIndex;

	// Panel Tab	
	switch (m_PanelTab)
	{
		// ����ҳ�� Tab
		case PanelTab_Control:
		{
			
			
			break;
		}
		
		// ͼ����ҳ�� Tab
		case PanelTab_ImagePro:
		{
			// ��ȡ��ǰ��ͼ������ҳ��
			m_ImageProcessTab 
				= (ImageProcessTab)ui.ImageProcessTabWidget->currentIndex();
			
			// ImageProcess Tab
			switch (m_ImageProcessTab)
			{
				// �Ҷȱ任 Tab
				case ImageProcessTab_GrayTrans:
				{
					// ���»Ҷȱ任 Tab
					GrayTransTabUpdate();
					
					// ���º괦�� Tab
					MarcoTabUpdate();

					break;
				}
				
				// ͼ����ǿ Tab
				case ImageProcessTab_Enhancement:
				{
					
					
					break;
				}
				
				default:
				{
					
					
					break;
				}
				
			}

			break;
		}
		
		default:
		{
			break;
		}
		
	}

	return;
}

/**************************************************************
�������� SwitchImageProcessTab

���� ��  �л� ImageProcess Tab

��ڲ��� ��
	int iIndex : Tab���

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::SwitchImageProcessTab(int iIndex)
{
	// ��������
	int iRet;
	ImageProcessTab m_ImageProcessTab;

	// ����ǰͼ��������ͼ����ܵ�
	if (true == bIsUpdateImageIns)
	{
		CurrProccessToPipe();
	}

	// ��ȡ��ǰͼ������ҳ��
	m_ImageProcessTab = (ImageProcessTab)iIndex;
	
	// ImageProcess Tab
	switch (m_ImageProcessTab)
	{
		// �Ҷȱ任 Tab
		case ImageProcessTab_GrayTrans:
		{
			// �Ҷȱ任�������
			GrayTransTabUpdate();
			
			// ��������
			MarcoTabUpdate();
			
			break;
		}
		
		// ͼ����ǿ Tab
		case ImageProcessTab_Enhancement:
		{
			// ��������
			MarcoTabUpdate();
			
			break;
		}

		default:
		{
			// ��������
			MarcoTabUpdate();

			break;
		}	
	}

	return;
}

/**************************************************************
�������� MarcoExcute

���� ��	 �����ִ�вۺ���

��ڲ��� ��
	QString StrMarcoLabel �� ͼ��괦��ܵ���ʶ

��������ֵ ��
	
****************************************************************/
void ZCIPSMainFrame::MarcoExcute(QString StrMarcoLabel)
{
	// ��������
	int iRet;
	bool bRet;
	QSharedPointer<ImageProPipe> pProPipe;
	
	// ��ǰͼ��ʵ���Ƿ�����ж�
	bRet = IsCurrImageInstanceValid();
	if (false == bRet) return;

	// ��ǰ�괦��ܵ���ȡ
	pProPipe = pMarco.get()->GetImageProPipe(StrMarcoLabel);
	if (true == pProPipe.isNull()) return;

	// �����ǰ����
	PairCurrImageIns.second.get()->ClearCurrProcess();

	// ͼ����ܵ�����
	iRet = PairCurrImageIns.second.get()->LoadImageProPipe(pProPipe);
	if (SUCCESS != iRet) return;
	
	// �����������
	UpdatePanel();

	// ͼ�����
	UpdateImage();
	
	return;
}

/**************************************************************
�������� MarcoDelete

���� ��	 ��ɾ���ۺ���

��ڲ��� ��
	QString StrMarcoLabel �� ͼ��괦��ܵ���ʶ

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::MarcoDelete(QString StrMarcoLabel)
{
	// ɾ����Ӧ�ĺ�
	pMarco.get()->DeleteMarco(StrMarcoLabel);

	// ��Ҫ���¿��ٴ���ť����
	FastBtnDisUpdate();
	
	return;
}

/**************************************************************
�������� MarcoFastBtn

���� ��	 �����ɿ��ٰ�ť

��ڲ��� ��
	QString StrMarcoLabel �� ͼ��괦��ܵ���ʶ

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::MarcoFastBtn(QString StrMarcoLabel)
{
	// ��������
	bool bIsBtn;

	// ��ȡ��ǰ���Ƿ���ڿ��ٰ�ť
	// Ȼ����з�ѡ����
	bIsBtn = pMarco.get()->IsFastBtn(StrMarcoLabel);
	if (true == bIsBtn) pMarco.get()->SetFastBtn(StrMarcoLabel, false);
	else pMarco.get()->SetFastBtn(StrMarcoLabel, true);

	// ���¿��ٴ���ť����
	FastBtnDisUpdate();
	
	return;
}

/**************************************************************
�������� SliderLowerValChanged

���� ��	 �Ҷȱ任���޻���ֵ�ı�

��ڲ��� �� 

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::SliderLowerValChanged()
{
	// ��������
	int iRet;
	float fBottomVal;
	float fPreBottomVal;
	GrayTransProcess *pGrayTransProcess;    // ��ṹ���ƣ��޷�ʹ������ָ��
											// �ش������������ע����ProcessPipe���

	// ��ȡ��ǰ�Ĵ���ʵ��
	// �޵�ǰͼ����ʵ���򴴽�ͼ����ʵ��
	// �е�ǰͼ����ʵ�����ȡ�ô���ʵ��
	if (true != PairCurrImageIns.second.get()->IsCurrProcessValid())
	{
		// �����Ҷȱ任ʵ��
		pGrayTransProcess = new GrayTransProcess();

		// ��ָ�븳ֵ����ǰ����ʵ��
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.first = ProcessType_GrayTrans;
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.second = (ImageProcess*)pGrayTransProcess;
	}
	else
	{
		pGrayTransProcess = (GrayTransProcess*)PairCurrImageIns.second.get()
			->pImageProcessIns.get()->PairCurrProcess.second;
	}

	// ���޸�ֵ
	fBottomVal = (float)m_GrayTransUI.pTSlider->iGetLowerVal();

	// �ж�����ֵ�Ƿ���
	fPreBottomVal = pGrayTransProcess->GetBottom();
	if (fPreBottomVal == fBottomVal) return;

	// �ж��Ƿ����ͼ������ˢ��
	if (true == bIsUpdatedDisplay)
	{
		// ���޲�������
		pGrayTransProcess->SetBottom((float)fBottomVal);
		
		// ͼ����
		iRet = pGrayTransProcess->ImagePro(
			PairCurrImageIns.second.get()->pImageData.get()->pfCurrData,
			PairCurrImageIns.second.get()->pImageData.get()->pfProcessingData,
			PairCurrImageIns.second.get()->pImageData.get()->iWidth,
			PairCurrImageIns.second.get()->pImageData.get()->iHeight);
		if (SUCCESS != iRet) return;

		// ͼ����ʾ
		UpdateImage();
		
		// ����Ϊ������Ч
		bIsUpdatedDisplay = false;

		// SpinBox��ֵ����
		m_GrayTransUI.pSpinBottom->setValue(fBottomVal);

		// ����Ϊ������Ч
		bIsUpdatedDisplay = true;
	}
	else
	{
		// SpinBox��ֵ����
		m_GrayTransUI.pSpinBottom->setValue(fBottomVal);
	}

	return;
}

/**************************************************************
�������� SliderMidValChanged

���� ��	 ���޻���ֵ�ı�

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::SliderMidValChanged()
{
	// ��������
	int iRet;
	int iBottom;
	int iMid;
	int iTop;
	float fR;
	float fPreR;
	GrayTransProcess *pGrayTransProcess;    // ��ṹ���ƣ��޷�ʹ������ָ��
											// �ش������������ע����ProcessPipe���

	// ��ȡ��ǰ�Ĵ���ʵ��
	// �޵�ǰͼ����ʵ���򴴽�ͼ����ʵ��
	// �е�ǰͼ����ʵ�����ȡ�ô���ʵ��
	if (true != PairCurrImageIns.second.get()->IsCurrProcessValid())
	{
		// �����Ҷȱ任ʵ��
		pGrayTransProcess = new GrayTransProcess();

		// ��ָ�븳ֵ����ǰ����ʵ��
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.first = ProcessType_GrayTrans;
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.second = (ImageProcess*)pGrayTransProcess;
	}
	else
	{
		pGrayTransProcess = (GrayTransProcess*)PairCurrImageIns.second.get()
			->pImageProcessIns.get()->PairCurrProcess.second;
	}

	// ����ֵ����
	iBottom = m_GrayTransUI.pTSlider->iGetLowerVal();
	iMid = m_GrayTransUI.pTSlider->iGetMidVal();
	iTop = m_GrayTransUI.pTSlider->iGetUpperVal();
	fR = TransR(iBottom, iMid, iTop);
	if (-1 == fR) return;

	// �ж�����ֵ�Ƿ���
	fPreR = pGrayTransProcess->GetR();
	if (fR == fPreR) return;

	// �ж��Ƿ����ͼ������ˢ��
	if (true == bIsUpdatedDisplay)
	{
		// ���޲�������
		pGrayTransProcess->SetR((float)fR);
		
		// ͼ����
		iRet = pGrayTransProcess->ImagePro(
			PairCurrImageIns.second.get()->pImageData.get()->pfCurrData,
			PairCurrImageIns.second.get()->pImageData.get()->pfProcessingData,
			PairCurrImageIns.second.get()->pImageData.get()->iWidth,
			PairCurrImageIns.second.get()->pImageData.get()->iHeight);
		if (SUCCESS != iRet) return;

		// ͼ����ʾ
		UpdateImage();

		// ��ʶλ����
		bIsUpdatedDisplay = false;

		// SpinBox��ֵ����
		m_GrayTransUI.pSpinR->setValue(fR);

		// ��ʶλ����
		bIsUpdatedDisplay = true;
	}
	else
	{
		// SpinBox��ֵ����
		m_GrayTransUI.pSpinR->setValue(fR);
	}

	return;
}

/**************************************************************
�������� SliderUpperValChanged

���� ��	 ���޻���ֵ�ı�

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::SliderUpperValChanged()
{
	// ��������
	int iRet;
	float fTopVal;
	float fPreTopVal;
	GrayTransProcess *pGrayTransProcess;    // ��ṹ���ƣ��޷�ʹ������ָ��
											// �ش������������ע����ProcessPipe���


	// ��ȡ��ǰ�Ĵ���ʵ��
	// �޵�ǰͼ����ʵ���򴴽�ͼ����ʵ��
	// �е�ǰͼ����ʵ�����ȡ�ô���ʵ��
	if (true != PairCurrImageIns.second.get()->IsCurrProcessValid())
	{
		// �����Ҷȱ任ʵ��
		pGrayTransProcess = new GrayTransProcess();

		// ��ָ�븳ֵ����ǰ����ʵ��
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.first = ProcessType_GrayTrans;
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.second = (ImageProcess*)pGrayTransProcess;
	}
	else
	{
		pGrayTransProcess = (GrayTransProcess*)PairCurrImageIns.second.get()
			->pImageProcessIns.get()->PairCurrProcess.second;
	}

	// ���޸�ֵ
	fTopVal = (float)m_GrayTransUI.pTSlider->iGetUpperVal();

	// �ж�����ֵ�Ƿ���
	fPreTopVal = pGrayTransProcess->GetTop();
	if (fPreTopVal == fTopVal) return;

	// �ж��Ƿ����ͼ������ˢ��
	if (true == bIsUpdatedDisplay)
	{
		// ���޲�������
		pGrayTransProcess->SetTop((float)fTopVal);
		
		// ͼ����
		iRet = pGrayTransProcess->ImagePro(
			PairCurrImageIns.second.get()->pImageData.get()->pfCurrData,
			PairCurrImageIns.second.get()->pImageData.get()->pfProcessingData,
			PairCurrImageIns.second.get()->pImageData.get()->iWidth,
			PairCurrImageIns.second.get()->pImageData.get()->iHeight);
		if (SUCCESS != iRet) return;

		// ͼ����ʾ
		UpdateImage();

		// ��ʶλ����
		bIsUpdatedDisplay = false;

		// SpinBox��ֵ����
		m_GrayTransUI.pSpinTop->setValue(fTopVal);

		// ��ʶλ����
		bIsUpdatedDisplay = true;
	}
	else
	{
		// SpinBox��ֵ����
		m_GrayTransUI.pSpinTop->setValue(fTopVal);
	}

	return;
}

/**************************************************************
�������� SpinBottomValChanged

���� ��	 ����Spinֵ�ı�

��ڲ��� ��
	double dInVal ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::SpinBottomValChanged(double dInVal)
{
	// ��������
	int iRet;
	float fPreBottomVal;
	float fPreTopVal;
	GrayTransProcess *pGrayTransProcess;    // ��ṹ���ƣ��޷�ʹ������ָ��
											// �ش������������ע����ProcessPipe���

	// ��ȡ��ǰ�Ĵ���ʵ��
	// �޵�ǰͼ����ʵ���򴴽�ͼ����ʵ��
	// �е�ǰͼ����ʵ�����ȡ�ô���ʵ��
	if (true != PairCurrImageIns.second.get()->IsCurrProcessValid())
	{
		// �����Ҷȱ任ʵ��
		pGrayTransProcess = new GrayTransProcess();

		// ��ָ�븳ֵ����ǰ����ʵ��
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.first = ProcessType_GrayTrans;
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.second = (ImageProcess*)pGrayTransProcess;
	}
	else
	{
		pGrayTransProcess = (GrayTransProcess*)PairCurrImageIns.second.get()
			->pImageProcessIns.get()->PairCurrProcess.second;
	}

	// ������ֵ
	fPreBottomVal = pGrayTransProcess->GetBottom();
	fPreTopVal = pGrayTransProcess->GetTop();

	// �������
	if ((0 > dInVal) || (dInVal >= (double)fPreTopVal - 10))
	{
		m_GrayTransUI.pSpinBottom->setValue(fPreBottomVal);
		return;
	}

	// Slider��������
	m_GrayTransUI.pTSlider->SetLowerVal((int)round(dInVal), true);

	// ��������ͬ����ͼ����Ҫ�ı�
	if ((float)dInVal == fPreBottomVal) return;

	// �ж��Ƿ���Ҫ����ͼ�����
	if (true == bIsUpdatedDisplay)
	{
		// ���޲�������
		pGrayTransProcess->SetBottom((float)dInVal);

		// ͼ����
		iRet = pGrayTransProcess->ImagePro(
			PairCurrImageIns.second.get()->pImageData.get()->pfCurrData,
			PairCurrImageIns.second.get()->pImageData.get()->pfProcessingData,
			PairCurrImageIns.second.get()->pImageData.get()->iWidth,
			PairCurrImageIns.second.get()->pImageData.get()->iHeight);
		if (SUCCESS != iRet) return;

		// ͼ����ʾ
		UpdateImage();
	}
}

/**************************************************************
�������� SpinRValChanged

���� ��	 RֵSpinֵ�ı�

��ڲ��� ��
	double dInVal ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::SpinRValChanged(double dInVal)
{
	// ��������
	int iRet;
	int iSliderLowerVal;
	int iSliderUpperVal;
	int iSliderMidVal;
	float fPreRVal;
	GrayTransProcess *pGrayTransProcess;    // ��ṹ���ƣ��޷�ʹ������ָ��
											// �ش������������ע����ProcessPipe���

	// ��ȡ��ǰ�Ĵ���ʵ��
	// �޵�ǰͼ����ʵ���򴴽�ͼ����ʵ��
	// �е�ǰͼ����ʵ�����ȡ�ô���ʵ��
	if (true != PairCurrImageIns.second.get()->IsCurrProcessValid())
	{
		// �����Ҷȱ任ʵ��
		pGrayTransProcess = new GrayTransProcess();

		// ��ָ�븳ֵ����ǰ����ʵ��
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.first = ProcessType_GrayTrans;
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.second = (ImageProcess*)pGrayTransProcess;
	}
	else
	{
		pGrayTransProcess = (GrayTransProcess*)PairCurrImageIns.second.get()
			->pImageProcessIns.get()->PairCurrProcess.second;
	}

	// ������ֵ
	fPreRVal = pGrayTransProcess->GetR();

	// �������
	if ((0.1 > dInVal) || (10 < dInVal))
	{
		m_GrayTransUI.pSpinR->setValue(fPreRVal);
		return;
	}

	// Slider��������
	iSliderLowerVal = m_GrayTransUI.pTSlider->iGetLowerVal();
	iSliderUpperVal = m_GrayTransUI.pTSlider->iGetUpperVal();
	iSliderMidVal = VerTransR(iSliderLowerVal, iSliderUpperVal, dInVal);
	m_GrayTransUI.pTSlider->SetMidVal(iSliderMidVal, true);

	// ��������ͬ����ͼ����Ҫ�ı�
	if ((float)dInVal == fPreRVal) return;

	// �ж��Ƿ���Ҫ����ͼ�����
	if (true == bIsUpdatedDisplay)
	{
		// Rֵ����
		pGrayTransProcess->SetR((float)dInVal);

		// ͼ����
		iRet = pGrayTransProcess->ImagePro(
			PairCurrImageIns.second.get()->pImageData.get()->pfCurrData,
			PairCurrImageIns.second.get()->pImageData.get()->pfProcessingData,
			PairCurrImageIns.second.get()->pImageData.get()->iWidth,
			PairCurrImageIns.second.get()->pImageData.get()->iHeight);
		if (SUCCESS != iRet) return;

		// ͼ����ʾ
		UpdateImage();
	}

	return;
}

/**************************************************************
�������� SpinTopValChanged

���� ��	 ����Spinֵ�ı�

��ڲ��� ��
	double dInVal ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::SpinTopValChanged(double dInVal)
{
	// ��������
	int iRet;
	float fPreBottomVal;
	float fPreTopVal;
	GrayTransProcess *pGrayTransProcess;    // ��ṹ���ƣ��޷�ʹ������ָ��
											// �ش������������ע����ProcessPipe���

	// ��ȡ��ǰ�Ĵ���ʵ��
	// �޵�ǰͼ����ʵ���򴴽�ͼ����ʵ��
	// �е�ǰͼ����ʵ�����ȡ�ô���ʵ��
	if (true != PairCurrImageIns.second.get()->IsCurrProcessValid())
	{
		// �����Ҷȱ任ʵ��
		pGrayTransProcess = new GrayTransProcess();

		// ��ָ�븳ֵ����ǰ����ʵ��
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.first = ProcessType_GrayTrans;
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.second = (ImageProcess*)pGrayTransProcess;
	}
	else
	{
		pGrayTransProcess = (GrayTransProcess*)PairCurrImageIns.second.get()
			->pImageProcessIns.get()->PairCurrProcess.second;
	}

	// ������ֵ
	fPreBottomVal = pGrayTransProcess->GetBottom();
	fPreTopVal = pGrayTransProcess->GetTop();

	// �������
	if ((255 < dInVal) || (dInVal <= (double)fPreBottomVal + 10))
	{
		m_GrayTransUI.pSpinTop->setValue(fPreTopVal);
		return;
	}

	// Slider��������
	m_GrayTransUI.pTSlider->SetUpperVal((int)round(dInVal), true);

	// ��������ͬ����ͼ����Ҫ�ı�
	if ((float)dInVal == fPreTopVal) return;

	// �ж��Ƿ���Ҫ����ͼ�����
	if (true == bIsUpdatedDisplay)
	{
		// ���޲�������
		pGrayTransProcess->SetTop((float)dInVal);

		// ͼ����
		iRet = pGrayTransProcess->ImagePro(
			PairCurrImageIns.second.get()->pImageData.get()->pfCurrData,
			PairCurrImageIns.second.get()->pImageData.get()->pfProcessingData,
			PairCurrImageIns.second.get()->pImageData.get()->iWidth,
			PairCurrImageIns.second.get()->pImageData.get()->iHeight);
		if (SUCCESS != iRet) return;

		// ͼ����ʾ
		UpdateImage();
	}

	return;
}

/**************************************************************
�������� GrayTransOK

���� ��	 Ӧ�ð�ť�ۺ���

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::GrayTransOK()
{
	// ��������
	int iRet;
	bool bRet;
	
	// ��ǰͼ��ʵ�����
	bRet = IsCurrImageInstanceValid();
	if (false == bRet) return;
	
	// ��ǰ������
	bRet = PairCurrImageIns.second.get()->IsCurrProcessValid();
	if (false == bRet) return;

	// ����ǰ����ʵ�����봦��ܵ�
	CurrProccessToPipe();

	// ����Panel����
	UpdatePanel();

	return;
}

/**************************************************************
�������� GrayTransCancel

���� ��	 ������ť�ۺ���

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::GrayTransCancel()
{
	// ��������
	int iRet;
	bool bRet;

	// ��ǰͼ��ʵ�����
	bRet = IsCurrImageInstanceValid();
	if (false == bRet) return;

	// ��ǰ������
	bRet = PairCurrImageIns.second.get()->IsCurrProcessValid();
	if (false == bRet) return;

	// �����ǰͼ����
	PairCurrImageIns.second.get()
		->pImageProcessIns.get()->ClearCurrProcess();

	// ��ǰ�����е�ͼ�񱻳���
	iRet = PairCurrImageIns.second.get()->pImageData.get()->ProcessingCancel();
	if (SUCCESS != iRet) return;

	// ����ͼ����ʾ
	UpdateImage();

	// ����Panel����
	UpdatePanel();

	return;
}

/**************************************************************
�������� UpdateImage

���� ��  ������ʾͼ������
         ���ݵ�ǰͼ��ʵ�������ݣ���ͼ����ʾ���ݽ��и���

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::UpdateImage()
{
	// ��������
	bool bRet;							// ����ֵ
	int iRet;                           // ����ֵ
	int iMdiHeight;						// Mdi�߶�
	int iMdiWidth;						// Mdi���
	QString StrLabel;					// ��ʶ
	ImageWidget *pSubWidget;				// �Ӵ���ָ��
	//QWidget *pSubWidget;
	// ������ֵ
	pSubWidget = nullptr;

	// �жϵ�ǰͼ��ʵ���Ƿ����
	bRet = IsCurrImageInstanceValid();
	if (true != bRet) return;

	// ��ʶ��ȡ
	StrLabel = PairCurrImageIns.first;

	// ��ȡ��ǰ������Widgetָ��
	switch (m_DisplayMode)
	{
		// ̽������ʾ
		case DisplayMode_Detector:
		{
			switch (m_DetectorNo)
			{
				// ��ǰ����Ϊ1��̽����
				case Detector_1:
				{
					if (nullptr != pScreenSplitter)
					{
						pSubWidget = (ImageWidget*)pScreenSplitter->widget(0);
					}
				
					break;
				}
				
				// ��ǰ����Ϊ2��̽����
				case Detector_2:
				{
					if (nullptr != pScreenSplitter)
					{
						pSubWidget = (ImageWidget*)pScreenSplitter->widget(1);
					}
					
					break;
				}
				
				default:
				{
					break;
				}
				
			}

			break;
		}
		
		// ͼ���б���ʾ
		case DisplayMode_ImageList:
		{
			// ��ȡ��ǰ������Ӵ���
			pSubWidget =(ImageWidget*) pScreenMdiArea->currentSubWindow();

			// �жϼ������ͼ��ʵ���Ƿ�ƥ��
			// ����ƥ�䣬��ȡ������
			if (StrLabel != pSubWidget->windowTitle())
			{
				return;
			}

			break;
		}

		case DisplayMode_None:
		default:
		{
			break;
		}
	}

	// ��ʼ��
	pImageDisplay.reset(new ImageDisplay());

	// �ж��Ƿ����Ӵ���
	// ��Ӧ�Ӵ��ڲ����ڣ����˳�
	if (nullptr == pSubWidget) return;
	
	// ��ͼ��ʵ��ת��Ϊͼ��
	iRet = pImageDisplay.get()->InstanceToDisplay(
		PairCurrImageIns.second,
		pSubWidget);
	if (SUCCESS != iRet) return;

	return;
}

/**************************************************************
�������� SeletctDet

���� ��  ѡ��̽����

��ڲ��� ��
	DetectorNo DetNo �� ̽�������

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::SeletctDet(DetectorNo DetNo)
{
	switch (DetNo)
	{
		case Detector_1:
		{
			// ��ǰѡ������
			m_DetectorNo = Detector_1;
			
			// �߿�����
			pScreenSplitter->widget(0)
				->setStyleSheet("background-color:white;border: 2px solid #33CCFF;");
			pScreenSplitter->widget(1)
				->setStyleSheet("background-color:white;border: 1px solid #C4C4C3;");

			break;
		}

		case Detector_2:
		{
			// ��ǰѡ������
			m_DetectorNo = Detector_2;

			// �߿�����
			pScreenSplitter->widget(1)
				->setStyleSheet("background-color:white;border: 2px solid #33CCFF;");
			pScreenSplitter->widget(0)
				->setStyleSheet("background-color:white;border: 1px solid #C4C4C3;");
	
			break;
		}
		
		default:
		{
			break;
		}
	}

	return;
}

/**************************************************************
�������� CurrProccessToPipe

���� ��  ����ǰ�������ͼ����ܵ�

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::CurrProccessToPipe()
{
	// ��������
	bool bRet;
	int iRet;

	// �жϵ�ǰͼ��ʵ���Ƿ����
	bRet = IsCurrImageInstanceValid();
	if (false == bRet) return;

	// �жϵ�ǰ�����Ƿ�����Ч�Ĳ����ı�
	bRet = IsCurrProValChanged();
	if (false == bRet) return;

	// ����ǰͼ��������ͼ����ܵ�
	iRet = PairCurrImageIns.second.get()->AddProcessPipe();
	if (SUCCESS != iRet) return;

	// pfCurrData -> pfProccessingData
	iRet = PairCurrImageIns.second.get()->DataProccessAccept();
	if (SUCCESS != iRet) return;

	return;
}
/**************************************************************
�������� InitialUI

���� ��  ��ʼ��UI����

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::InitialUI()
{
	// ��������
	QRect RectScreen;
	int iHeight;

	// ָ�븳ֵ

	// UI����
	this->setWindowState(Qt::WindowMaximized);
	this->setWindowFlags(this->windowFlags()
		&~Qt::WindowMaximizeButtonHint
		&~Qt::WindowMinMaxButtonsHint);
	RectScreen = QApplication::desktop()->availableGeometry();
	this->setFixedHeight(RectScreen.height()- 20);
	this->setFixedWidth(RectScreen.width());
	ui.PanelDockWidget->setFixedWidth(400);
	ui.PanelDockWidget->setFixedHeight(this->height() - 20);
	ui.PanelTabWidget->setFixedWidth(395);
	ui.PanelTabWidget->setFixedHeight(ui.PanelDockWidget->height() - 40);
	ui.ImageProcessTabWidget->setFixedWidth(390);
	ui.ImageProcessTabWidget->setFixedHeight(ui.PanelTabWidget->height() - 30 - 300);
	ui.MarcoBox->setGeometry(ui.ImageProcessTabWidget->x(), 
		ui.ImageProcessTabWidget->y() + ui.ImageProcessTabWidget->height() + 5,
		388, 300);
	ui.GrayTrans->setGeometry(0, 0, ui.PanelTabWidget->width() - 10, ui.PanelTabWidget->height());
	ui.Enhancement->setGeometry(0, 0, ui.PanelTabWidget->width() - 10, ui.PanelTabWidget->height());

	// �Ҷȱ任�����ʼ��
	GrayTransUIInit();

	// �괦������ʼ��
	MarcoUIInit();

	// ͼ����ʾ��ʼ��
	InitialImgDisplay();

	// ��ʾ��������
	ui.centralWidget->setMouseTracking(true);

	// �����ۺ������źŵ����� 
	connect(ui.PanelTabWidget, SIGNAL(currentChanged(int)), this, SLOT(SwitchPanelTab(int)));
	connect(ui.ImageProcessTabWidget, SIGNAL(currentChanged(int)), this, SLOT(SwitchImageProcessTab(int)));
	//���ݿ����
	DataBaseUIInit();


	auto controlWidget = CtScanExport::loadCtScanWidget(ui.DeviceControlTab);

}

/**************************************************************
�������� MarcoUIInit

���� ��  �괦���ʼ��UI����

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::MarcoUIInit()
{
	// ��ʼ��
	m_MarcoUI.pMarcoPipeArea = new MarcoPipeArea(ui.MarcoBox);
	m_MarcoUI.pFastBtnArea = new FastBtnArea(ui.MarcoBox);
	m_MarcoUI.pBtnMarcoGenerate = ui.ButtonMarcoGenerate;
	m_MarcoUI.pBtnMarcoLoad = ui.ButtonMarcoLoad;
	m_MarcoUI.pBtnMarcoClear = ui.ButtonMarcoClear;
	m_MarcoUI.pBtnPipeCut = ui.ButtonPipleCut;
	m_MarcoUI.pBtnFastEdit = ui.ButtonFastEdit;

	// ����λ������ 
	m_MarcoUI.pBtnMarcoLoad->setGeometry(10, 25, 84, 25);
	m_MarcoUI.pBtnMarcoGenerate->setGeometry(
		10 + m_MarcoUI.pBtnMarcoLoad->width() + 10, 25, 84, 25);
	m_MarcoUI.pBtnPipeCut->setGeometry(
		10 + m_MarcoUI.pBtnMarcoLoad->width() 
		+ 10 + m_MarcoUI.pBtnMarcoGenerate->width() + 10,
		25, 84, 25);
	m_MarcoUI.pBtnMarcoClear->setGeometry(
		10 + m_MarcoUI.pBtnMarcoLoad->width() 
		+ 10 + m_MarcoUI.pBtnMarcoGenerate->width() 
		+ 10 + m_MarcoUI.pBtnPipeCut->width() + 10,
		25, 84, 25);
	m_MarcoUI.pMarcoPipeArea->setGeometry(10,
		25 + m_MarcoUI.pBtnMarcoLoad->height() + 10,
		ui.MarcoBox->width() - 20, 130);
	m_MarcoUI.pMarcoPipeArea->widget()->setGeometry(0, 0,
		m_MarcoUI.pMarcoPipeArea->width(),
		m_MarcoUI.pMarcoPipeArea->height() - 20);
	m_MarcoUI.pFastBtnArea->setGeometry(10,
		25 + m_MarcoUI.pBtnMarcoLoad->height()
		+ 10 + m_MarcoUI.pMarcoPipeArea->height() + 20,
		m_MarcoUI.pMarcoPipeArea->width() - 80, 80);
	m_MarcoUI.pBtnFastEdit->setGeometry(
		10 + m_MarcoUI.pFastBtnArea->width(),
		25 + m_MarcoUI.pBtnMarcoLoad->height()
		+ 10 + m_MarcoUI.pMarcoPipeArea->height() + 20,
		(m_MarcoUI.pMarcoPipeArea->width() - m_MarcoUI.pFastBtnArea->width()), 80);
	
	// ���ٰ�ť�������
	FastBtnDisUpdate();

	// �ؼ�ʹ��
	MarcoEnableCtrl();

	// �ۺ������ź�����
	connect(m_MarcoUI.pBtnMarcoGenerate, SIGNAL(clicked(bool)),
		this, SLOT(GenerateMarcoSlot(bool)));
	connect(m_MarcoUI.pBtnMarcoLoad, SIGNAL(clicked(bool)),
		this, SLOT(LoadMarcoSlot(bool)));
	connect(m_MarcoUI.pBtnFastEdit, SIGNAL(clicked(bool)),
		this, SLOT(LoadMarcoSlot(bool)));
	connect(m_MarcoUI.pBtnPipeCut, SIGNAL(clicked(bool)),
		this, SLOT(CutSlot(bool)));
	connect(m_MarcoUI.pBtnMarcoClear, SIGNAL(clicked(bool)),
		this, SLOT(ClearSlot(bool)));

	return;
}

/**************************************************************
�������� UpdateMarcoPiple

���� ��  ���º괦��ܵ���ʾ

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::MarcoTabUpdate()
{
	// ����ܵ���ʾ����
	PipeDisUpdate();

	// ���ٴ���ť��ʾ����
	FastBtnDisUpdate();

	// �ؼ�ʹ��
	MarcoEnableCtrl();
	
}

/**************************************************************
�������� PipeDisUpdate

���� ��  ���´���ܵ���ʾ

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::PipeDisUpdate()
{
	// ��������
	int iImgProcessSize;
	int iRet;
	bool bRet;
	QString StrPara;
	QString StrProcessName;
	ProcessType InType;
	ImageProcess *pImageProcess;
	QTextCodec *pTCode;

	// �жϵ�ǰͼ����ʵ���Ƿ����
	bRet = IsCurrImageInstanceValid();
	if (false == bRet)
	{
		// ���õ�ǰ�ĺ괦��ܵ���ʾ
		m_MarcoUI.pMarcoPipeArea->ClearPipe();

		return;
	}

	// ���õ�ǰ�ĺ괦��ܵ���ʾ
	m_MarcoUI.pMarcoPipeArea->ClearPipe();

	// ��ȡ��ǰ�괦��ܵ��ĳ���
	iImgProcessSize = PairCurrImageIns.second.get()->pImageProcessIns.get()
		->pProcessPipe.get()->GetPipeLen();

	// ������괦��ͨ��
	for (int i = 0; i < iImgProcessSize; i++)
	{
		// ��ȡ�������������
		iRet = PairCurrImageIns.second.get()
			->pImageProcessIns.get()
			->pProcessPipe.get()->PipeGet
			(i, InType, &pImageProcess);
		if (SUCCESS != iRet) return;

		// ��ȡ�����ַ�
		switch (InType)
		{
			case ProcessType_GrayTrans:
			{
				// ��ȡ����
				iRet = pImageProcess->GetParaString(StrPara);
				if (SUCCESS != iRet) return;

				// ��������
				pTCode = QTextCodec::codecForName("GBK");
				StrProcessName = pTCode->toUnicode("�Ҷȱ任");

				break;
			}

			case ProcessType_Enhancement:
			{
				continue;
			}

			case ProcessType_None:
			default:
			{
				continue;
			}
		}

		// ���ؽ���ScrollArea�ؼ�
		m_MarcoUI.pMarcoPipeArea->AddPipeUnit(StrProcessName, StrPara);
	}

	// ѡ��
	m_MarcoUI.pMarcoPipeArea->SelectWidget();

	return;
}

/**************************************************************
�������� FastBtnDisUpdate

���� ��  ���ٴ���ť��ʾ����

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::FastBtnDisUpdate()
{
	// ��������
	QVector<FastButton *>::iterator it;
	QVector<FastButton *> vecFastBtn;
	
	// ��������ѡ��
	m_MarcoUI.pFastBtnArea->FastBtnUpdate(pMarco);

	// ������Ϣ��ۺ���������
	vecFastBtn = m_MarcoUI.pFastBtnArea->GetBtnVec();
	for (it = vecFastBtn.begin(); it != vecFastBtn.end(); it++)
	{
		connect(*it, SIGNAL(FastBtn(QString)), this, SLOT(MarcoExcute(QString)));
	}
	
	return;
}

/**************************************************************
�������� EnableCtrl

���� ��  �ؼ�ʹ��

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::MarcoEnableCtrl()
{
	// �Ƿ������ڴ���ͼ��ʵ��
	if (true == IsCurrImageInstanceValid())
	{
		// �Ƿ���ڴ���ܵ�
		if (true == PairCurrImageIns.second.get()->IsProcessPipeValid())
		{
			m_MarcoUI.pBtnMarcoGenerate->setEnabled(true);
			m_MarcoUI.pBtnMarcoLoad->setEnabled(true);
			m_MarcoUI.pBtnPipeCut->setEnabled(true);
			m_MarcoUI.pBtnMarcoClear->setEnabled(true);
			m_MarcoUI.pBtnFastEdit->setEnabled(true);
			m_MarcoUI.pMarcoPipeArea->setEnabled(true);
			m_MarcoUI.pFastBtnArea->setEnabled(true);
		}
		else
		{
			m_MarcoUI.pBtnMarcoGenerate->setEnabled(false);
			m_MarcoUI.pBtnMarcoLoad->setEnabled(true);
			m_MarcoUI.pBtnPipeCut->setEnabled(false);
			m_MarcoUI.pBtnMarcoClear->setEnabled(false);
			m_MarcoUI.pBtnFastEdit->setEnabled(true);
			m_MarcoUI.pMarcoPipeArea->setEnabled(true);
			m_MarcoUI.pFastBtnArea->setEnabled(true);
		}
	}
	else
	{
		m_MarcoUI.pBtnMarcoGenerate->setEnabled(false);
		m_MarcoUI.pBtnMarcoLoad->setEnabled(true);
		m_MarcoUI.pBtnPipeCut->setEnabled(false);
		m_MarcoUI.pBtnMarcoClear->setEnabled(false);
		m_MarcoUI.pBtnFastEdit->setEnabled(true);
		m_MarcoUI.pMarcoPipeArea->setEnabled(false);
		m_MarcoUI.pFastBtnArea->setEnabled(false);
	}
}

 
/**************************************************************
�������� GenerateMarcoSlot

���� ��  �����ɲۺ���

��ڲ��� ��
	bool bChecked ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::GenerateMarcoSlot(bool bChecked)
{
	// �������� 
	bool bRet;
	AddMarcoWidget *pGenWidget;

	// �жϵ�ǰͼ��ʵ���Ƿ����
	bRet = IsCurrImageInstanceValid();
	if (true != bRet) return;

	// ��ʼ��
	pGenWidget = new AddMarcoWidget(
		pMarco, 
		PairCurrImageIns.second,
		nullptr);

	// �򿪶Ի���
	pGenWidget->show();

	return;
}

/**************************************************************
�������� GenerateMarcoSlot

���� ��  �����/�༭�ۺ���

��ڲ��� ��
	bool bChecked ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::LoadMarcoSlot(bool bChecked)
{
	// �������� 
	LoadMarcoWidget *pLoadMarcoWidget;
	
	// ��ʼ��
	pLoadMarcoWidget = new LoadMarcoWidget(
		PairCurrImageIns.second,
		pMarco,
		nullptr);

	// �ۺ������ź�����
	connect(pLoadMarcoWidget, SIGNAL(ExcuteEmit(QString)),
		this, SLOT(MarcoExcute(QString)));
	connect(pLoadMarcoWidget, SIGNAL(DeleteEmit(QString)),
		this, SLOT(MarcoDelete(QString)));
	connect(pLoadMarcoWidget, SIGNAL(FastBtnEmit(QString)),
		this, SLOT(MarcoFastBtn(QString)));

	// �򿪶Ի���
	pLoadMarcoWidget->show();
}

/**************************************************************
�������� CutSlot

���� ��  ��ȡ�ۺ���

��ڲ��� ��
	bool bChecked ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::CutSlot(bool bChecked)
{
	// ��������
	int iRet;
	bool bRet;         // ��������ֵ
	int iPipeSize;
	int iSelProNo; 
	
	// �жϵ�ǰͼ��ʵ���Ƿ����
	bRet = IsCurrImageInstanceValid();
	if (false == bRet) return;

	// �жϴ���ܵ��Ƿ����
	bRet = PairCurrImageIns.second.get()->IsProcessPipeValid();
	if (false == bRet) return;

	// ��ȡ��ǰ����ܵ�����
	iPipeSize = PairCurrImageIns.second.get()->ProPipeSize();
	if (0 >= iPipeSize) return;

	// ��ȡ��ǰѡ��Ĵ�������
	// �������ж�
	iSelProNo = m_MarcoUI.pMarcoPipeArea->GetCurrSel();
	if ((0 > iSelProNo) || (iPipeSize <= iSelProNo)) return;
	
	// �����ǰ����
	PairCurrImageIns.second.get()->ClearCurrProcess();

	// �ܵ���ȡ
	iRet = PairCurrImageIns.second.get()->ProPipeCut(iSelProNo);
	if (SUCCESS != iRet) return;

	// ִ���µ�ͼ����ܵ�
	// ����ͼ����
	iRet = PairCurrImageIns.second.get()->ProPipeExcute();
	if (SUCCESS != iRet) return;

	// �����������
	UpdatePanel();

	// ͼ�����
	UpdateImage();
	
	return;
}

/**************************************************************
�������� ClearSlot

���� ��  ����ۺ���

��ڲ��� ��
	bool bChecked ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::ClearSlot(bool bChecked)
{	
	// �����ǰ����
	PairCurrImageIns.second.get()->ClearCurrProcess();

	// ����ܵ�����
	PairCurrImageIns.second.get()->ClearProPipe();

	// ��������
	PairCurrImageIns.second.get()->ResetImgData();

	// �����������
	UpdatePanel();

	// ͼ�����
	UpdateImage();

	return;
}


/**************************************************************
�������� GrayTranUIInit

���� ��  �Ҷȱ任��ʼ��UI����

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::GrayTransUIInit()
{
	// ��ʼ��
	m_GrayTransUI.pSpinBottom = ui.SpinBottomVal;
	m_GrayTransUI.pSpinR = ui.SpinRVal;
	m_GrayTransUI.pSpinTop = ui.SpinTopVal;
	m_GrayTransUI.pButtonCancel = ui.ButtonCancel;
	m_GrayTransUI.pButtonOK = ui.ButtonOK;
	
	m_GrayTransUI.pChartGray = new QChart();
	m_GrayTransUI.pChartViewGray = new QChartView(m_GrayTransUI.pChartGray, ui.GrayTrans);
	m_GrayTransUI.pTSlider = new TripleSlider(Qt::Horizontal, ui.GrayTrans);

	// ����λ������
	m_GrayTransUI.pChartViewGray->setGeometry(
		(ui.GrayTrans->width() - 350) / 2, 0, 350, 300);
	m_GrayTransUI.pTSlider->setGeometry(
		(ui.GrayTrans->width() - 340) / 2, m_GrayTransUI.pChartViewGray->height(), 340, 20);
	m_GrayTransUI.pSpinBottom->setGeometry(
		(ui.GrayTrans->width() - 350) / 2,
		m_GrayTransUI.pChartViewGray->height() + m_GrayTransUI.pTSlider->height() + 10,
		60, 20);
	m_GrayTransUI.pSpinR->setGeometry(
		(ui.GrayTrans->width() / 2) - 30, 
		m_GrayTransUI.pChartViewGray->height() + m_GrayTransUI.pTSlider->height() + 10,
		60, 20);
	m_GrayTransUI.pSpinTop->setGeometry(
		ui.GrayTrans->width() - ((ui.GrayTrans->width() - 350) / 2) - 60,
		m_GrayTransUI.pChartViewGray->height() + m_GrayTransUI.pTSlider->height() + 10,
		60, 20);
	m_GrayTransUI.pButtonCancel->setGeometry(
		(ui.GrayTrans->width() - 350) / 2,
		m_GrayTransUI.pChartViewGray->height() 
		+ m_GrayTransUI.pTSlider->height() + 10 
		+ m_GrayTransUI.pSpinBottom->height() + 20,
		80, 30);
	m_GrayTransUI.pButtonOK->setGeometry(
		(ui.GrayTrans->width() - 350) / 2 + 10
		+ m_GrayTransUI.pButtonCancel->width(),
		m_GrayTransUI.pChartViewGray->height()
		+ m_GrayTransUI.pTSlider->height() + 10
		+ m_GrayTransUI.pSpinBottom->height() + 20,
		80, 30);

	// ���ÿؼ�������
	EnableGrayTransCtrl(false);

	// ��ʾ
	m_GrayTransUI.pTSlider->show();

	// Spinbox����
	m_GrayTransUI.pSpinBottom->setRange(0, 245);
	m_GrayTransUI.pSpinBottom->setValue(0);
	m_GrayTransUI.pSpinBottom->setSingleStep(1);
	m_GrayTransUI.pSpinTop->setRange(10, 255);
	m_GrayTransUI.pSpinTop->setValue(255);
	m_GrayTransUI.pSpinTop->setSingleStep(1);
	m_GrayTransUI.pSpinR->setRange(0.1, 10);
	m_GrayTransUI.pSpinR->setValue(1);
	m_GrayTransUI.pSpinR->setSingleStep(0.1);
	m_GrayTransUI.pSpinR->setDecimals(4);

	// �����ۺ������źŵ�����
	connect(m_GrayTransUI.pTSlider, SIGNAL(LowerValueChanged()), this, SLOT(SliderLowerValChanged()));
	connect(m_GrayTransUI.pTSlider, SIGNAL(MidValueChanged()), this, SLOT(SliderMidValChanged()));
	connect(m_GrayTransUI.pTSlider, SIGNAL(UpperValueChanged()), this, SLOT(SliderUpperValChanged()));
	connect(m_GrayTransUI.pSpinBottom, SIGNAL(valueChanged(double)), this, SLOT(SpinBottomValChanged(double)));
	connect(m_GrayTransUI.pSpinTop, SIGNAL(valueChanged(double)), this, SLOT(SpinTopValChanged(double)));
	connect(m_GrayTransUI.pSpinR, SIGNAL(valueChanged(double)), this, SLOT(SpinRValChanged(double)));
	connect(m_GrayTransUI.pButtonOK, SIGNAL(clicked()), this, SLOT(GrayTransOK()));
	connect(m_GrayTransUI.pButtonCancel, SIGNAL(clicked()), this, SLOT(GrayTransCancel()));

}

/**************************************************************
�������� GrayTransTabUpdate

���� ��  

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::GrayTransTabUpdate()
{
	// ��������ֵ
	bool bRet;
	ProcessType m_PType;
	GrayTransProcess *pGranTransProcess;

	// �Ҷȱ任��������
	GrayTransTabReset();
	
	// �жϵ�ǰͼ��ʵ���Ƿ����
	// ��������ڣ��򷵻�
	bRet = IsCurrImageInstanceValid();
	if (false == bRet) return;

	// ����ͼ��������ֱ��ͼ
	ImageDataToHistogram(
		PairCurrImageIns.second.get()->pImageData.get()->pfCurrData,
		PairCurrImageIns.second.get()->pImageData.get()->iWidth,
		PairCurrImageIns.second.get()->pImageData.get()->iHeight,
		pAreaSelect);

	// ��ťʹ��
	EnableGrayTransCtrl(true);

	// �жϵ�ǰͼ�����Ƿ����
	// �����ݲ�ͬ������в�ͬ����
	bRet = PairCurrImageIns.second.get()->IsCurrProcessValid();
	m_PType = PairCurrImageIns.second.get()
		->pImageProcessIns.get()->PairCurrProcess.first;
	if ((true == bRet) && (ProcessType_GrayTrans == m_PType))
	{
		// ��ȡ��ǰͼ����ָ��
		pGranTransProcess = 
			(GrayTransProcess *)PairCurrImageIns.second.get()
			->pImageProcessIns.get()->PairCurrProcess                                                                                                          .second;
		
		bIsUpdatedDisplay = false;

		// �����޸�
		m_GrayTransUI.pSpinBottom->setValue(
			(double)pGranTransProcess->GetBottom());
		m_GrayTransUI.pSpinTop->setValue(
			(double)pGranTransProcess->GetTop());
		m_GrayTransUI.pSpinR->setValue(
			(double)pGranTransProcess->GetR());

		bIsUpdatedDisplay = true;
	}
	return;
}

/**************************************************************
�������� EnableGrayTransCtrl

���� ��  ʹ�ܻ��ؼ�
	
��ڲ��� ��
	bool bEnable :		ʹ�ܱ�ʶ

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::EnableGrayTransCtrl(bool bEnable)
{
	if (true == bEnable)
	{
		m_GrayTransUI.pSpinBottom->setEnabled(true);
		m_GrayTransUI.pSpinR->setEnabled(true);
		m_GrayTransUI.pSpinTop->setEnabled(true);
		m_GrayTransUI.pTSlider->setEnabled(true);
		m_GrayTransUI.pButtonCancel->setEnabled(true);
		m_GrayTransUI.pButtonOK->setEnabled(true);
	}
	else
	{
		m_GrayTransUI.pSpinBottom->setEnabled(false);
		m_GrayTransUI.pSpinR->setEnabled(false);
		m_GrayTransUI.pSpinTop->setEnabled(false);
		m_GrayTransUI.pTSlider->setEnabled(false);
		m_GrayTransUI.pButtonCancel->setEnabled(false);
		m_GrayTransUI.pButtonOK->setEnabled(false);
	}
}

/**************************************************************
�������� ImageDataToHistogram

���� ��  ͼ�����ݼ��ص�ֱ��ͼ

��ڲ��� ��
	QSharedPointer<float> pfData ��
	int iWidth ��
	int iHeight ��
	QSharedPointer<AreaSelect> pArea ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::ImageDataToHistogram
(
	QSharedPointer<float> pfData,
	int iWidth,
	int iHeight,
	QSharedPointer<AreaSelect> pArea
)
{
	// ��������
	int iGrayVal;
	float afHistogram[256];
	float fMax;
	QLineSeries *pLineGray;
	QAreaSeries *pAreaGray;
	QSharedPointer<UCHAR> pucAreaMask;

	// �����ж�
	if (true == pArea.isNull())
	{
		pucAreaMask.reset(new UCHAR[iHeight * iWidth]);
		memset(pucAreaMask.get(), 1, iHeight * iWidth * sizeof(UCHAR));
	}
	else
	{
		pucAreaMask = pArea.get()->pucAreaMask;
	}

	// �Ҷȷֲ�ͳ��
	memset(afHistogram, 0, 255 * sizeof(float));
	for (int i = 0; i < iHeight; i++)
		for (int j = 0; j < iWidth; j++)
		{
			if ((UCHAR)1 == pucAreaMask.get()[i * iWidth + j])
			{
				// �Ҷ�ֵȡ��
				iGrayVal = (int)round(pfData.get()[i * iWidth + j]);

				// ���ǻҶȷֲ�ͼ�Ŀɿ���
				// [0]��[255]����ͳ��
				if ((0 == iGrayVal) || (255 == iGrayVal)) continue;

				// ͳ��ֵ��ֵ
				afHistogram[iGrayVal] += (float)1;
			}
		}

	// �Ҷ�ֵ��һ���� 0 - 1
	fMax = afHistogram[0];
	for (int i = 1; i < 256; i++)
	{
		fMax = qMax(fMax, afHistogram[i]);
	}
	for (int i = 0; i < 256; i++)
	{
		afHistogram[i] = afHistogram[i] / fMax;
	}

	// ֱ��ͼͼ������
	m_GrayTransUI.pChartGray->removeAllSeries();

	// �Ҷ�ֱ��ͼ����
	pLineGray = new QLineSeries(m_GrayTransUI.pChartGray);
	for (int i = 0; i < 256; i++)
	{
		pLineGray->append(QPointF(i, afHistogram[i]));
	}
	pAreaGray = new QAreaSeries(pLineGray, nullptr);
	m_GrayTransUI.pChartGray->addSeries(pAreaGray);

	// Chart����
	m_GrayTransUI.pChartGray->createDefaultAxes();
	m_GrayTransUI.pChartGray->axisX()->setRange(0, 255);
	m_GrayTransUI.pChartGray->axisY()->setRange(0, 1);
	m_GrayTransUI.pChartGray->removeAxis(m_GrayTransUI.pChartGray->axisY());
	m_GrayTransUI.pChartGray->removeAxis(m_GrayTransUI.pChartGray->axisX());

	// ��Ե����
	m_GrayTransUI.pChartGray->setMargins(QMargins(0, 0, 0, 0));
	
	return;
}

/**************************************************************
�������� TransR

���� ��  ���ؼ��ϵ����޻���λ��ת��Ϊʵ�ʵ�Rֵ

��ڲ��� ��
	int iBottom ��
	int iMid ��
	int iTop ��

��������ֵ ��
	float fR:  ���� -- -1
			   ��ȷ -- 0.1 - 10
****************************************************************/
float ZCIPSMainFrame::TransR
(
	int iBottom,
	int iMid,
	int iTop
)
{
	// ��������
	int iRange;
	int iHandlePos;
	float fR;
	float fInterval;

	// �������
	if ((iTop < iBottom + 10) || (iMid <= iBottom) || (iMid >= iTop))
	{
		return -1;
	}

	// ������ֵ
	iRange = iTop - iBottom - 1;
	iHandlePos = iMid - iBottom - 1;

	// ����פ��������Ϊż�����м��������Ӧ R = 1
	// ����פ��������Ϊ�������м䵥�����Ӧ R = 1
	if (0 == (iRange % 2))		// ����פ��������Ϊż��
	{
		// ������
		fInterval = (float)1 / (float)((iRange / 2) - 1);

		// ����Rֵ
		if ((float)iHandlePos < ((float)(iRange - 1) / (float)2))   // �ϰ���
		{
			fR = pow(10, (float)(iHandlePos - (iRange / 2 - 1)) * fInterval);
		}
		else                                                        // �°���
		{
			fR = pow(10, (float)(iHandlePos - (iRange / 2)) * fInterval);
		}

	}
	else  // ����פ��������Ϊ����
	{
		// ������
		fInterval = (float)1 / (float)((iRange - 1) / 2);

		// ����Rֵ
		fR = pow(10, (float)(iHandlePos - ((iRange - 1) / 2)) * fInterval);	
	}

	return fR;
}

/**************************************************************
�������� VerTransR

���� ��  ���ؼ��ϵ����޻���λ��ת��Ϊʵ�ʵ�Rֵ

��ڲ��� ��
	int iBottom ��
	int iTop ��
	float fR ��

��������ֵ ��
	int iMid:  ���� -- -1
****************************************************************/
int ZCIPSMainFrame::VerTransR
(
	int iBottom,
	int iTop,
	float fR
)
{
	// ��������
	int iMid;
	int iRange;
	float fInterval;
	float fLogR;

	// �������
	if (iTop < iBottom + 10) return -1;

	// ������ֵ
	iRange = iTop - iBottom - 1;
	fLogR = log10(fR);

	// ����פ��������Ϊż�����м��������Ӧ R = 1
	// ����פ��������Ϊ�������м䵥�����Ӧ R = 1
	if (0 == (iRange % 2))		// ����פ��������Ϊż��
	{
		// ������
		fInterval = (float)1 / (float)((iRange / 2) - 1);

		// ����Midֵ
		if ((fLogR / fInterval) <= 0)
		{
			iMid = (iRange / 2) + (int)round(fLogR / fInterval);
		}
		else
		{
			iMid = (iRange / 2 + 1) + (int)round(fLogR / fInterval);
		}
	}
	else  // ����פ��������Ϊ����
	{
		// ������
		fInterval = (float)1 / (float)((iRange - 1) / 2);

		// ����Midֵ
		iMid = (iRange + 1) / 2 + (int)round(fLogR / fInterval);
	}

	return iBottom + iMid;
}

/**************************************************************
�������� GrayTransTabReset

���� ��  �Ҷȱ任��������

��ڲ��� ��	
	

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::GrayTransTabReset()
{
	
	// �ر�ͼ����
	bIsUpdatedDisplay = false;
	
	// �رտؼ�������
	EnableGrayTransCtrl(false);

	// ֱ��ͼͼ������
	m_GrayTransUI.pChartGray->removeAllSeries();

	// ����Ϊ��ʼֵ
	m_GrayTransUI.pSpinBottom->setValue(0);
	m_GrayTransUI.pSpinTop->setValue(255);
	m_GrayTransUI.pSpinR->setValue(1);
	
	// ����ͼ����
	bIsUpdatedDisplay = true;

	return;
}

/**************************************************************
�������� DisplaySwitch

���� ��  ��ʾģʽת��

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::DisplaySwitch()
{
	// ��������
	int iRet;
	int iMdiHeight;
	int iMdiWidth;
	QString StrPath;
	//QWidget *pWidget_1;						// ̽������ʾ����1
	//QWidget *pWidget_2;						// ̽������ʾ����2
	//QWidget *pSubWidget;
	ImageWidget *pWidget_1;
	ImageWidget *pWidget_2;
	ImageWidget *pSubWidget;
	QLabel *pLabel_1;
	QLabel *pLabel_2;
	QString StrLabel_1;
	QString StrLabel_2;

	// 
	switch (m_DisplayMode)
	{
		// ̽������ʾ
		case DisplayMode_Detector:
		{
			// ɾ��֮ǰ�Ŀؼ�
			if (nullptr != pScreenMdiArea)
			{
				pScreenMdiArea->closeAllSubWindows();
				delete pScreenMdiArea;
				pScreenMdiArea = nullptr;
			}
			if (nullptr != pScreenSplitter)
			{
				pScreenSplitter->close();
				delete pScreenSplitter;
				pScreenSplitter = nullptr;
			}

			// ��ʼ��Spliter
			pScreenSplitter = new QSplitter(Qt::Horizontal, ui.centralWidget);
			pScreenSplitter->setGeometry(0, 0,
				ui.centralWidget->width() - 30,
				ui.centralWidget->height());
			pScreenSplitter->setMouseTracking(true);
			
			// ����̽����1ͼ��
			if (false == IsDetInstanceValid(Detector_1))    // ̽����1û��ͼ��ʵ��
			{
				pWidget_1 = new ImageWidget(pScreenSplitter);
				pWidget_1->setStyleSheet("background-color:white;border: 1px solid #C4C4C3;");
				pWidget_1->setFixedSize(
					pScreenSplitter->width() / 2, 
					pScreenSplitter->height());
				StrLabel_1 = "Detector 1 Empty";
				pLabel_1 = new QLabel(pWidget_1);
				pLabel_1->setGeometry(0, 0, pWidget_1->width(), 20);
				pLabel_1->setText("Detector 1");
				pLabel_1->setAlignment(Qt::AlignCenter);
				pWidget_1->setWindowTitle(StrLabel_1);
				pScreenSplitter->addWidget(pWidget_1);
			}
			else                     // ̽����1��ͼ��ʵ��
			{
				// ��ʼ��
				pImageDisplay.reset(new ImageDisplay()); 
				pWidget_1 = new ImageWidget();
				pLabel_1 = new QLabel(pWidget_1);
				
				// ���ڲ�������
				pWidget_1->setWindowTitle(PairDetIns_1.first);
				pWidget_1->setGeometry(0, 0, 
					pScreenSplitter->width() / 2, 
					pScreenSplitter->height());
				pLabel_1->setText(PairDetIns_1.first);
				pLabel_1->setGeometry(0, 0, pWidget_1->width(), 20);
				pLabel_1->setAlignment(Qt::AlignCenter);
				
				// ͼ��ʵ��ת��ΪWidget
				iRet = pImageDisplay.get()->InstanceToDisplay(PairDetIns_1.second, pWidget_1);
				if (SUCCESS != iRet) return;

				// ���ش�����Splitter
				pScreenSplitter->addWidget(pWidget_1);
			}

			// ����̽����2ͼ��
			if (false == IsDetInstanceValid(Detector_2))    // ̽����2û��ͼ��ʵ��
			{
				pWidget_2 = new ImageWidget(pScreenSplitter);
				pWidget_2->setStyleSheet("background-color:white;border: 1px solid #C4C4C3;");
				pWidget_2->setFixedSize(
					pScreenSplitter->width() / 2,
					pScreenSplitter->height());
				StrLabel_2 = "Detector 2 Empty";
				pLabel_2 = new QLabel(pWidget_2);
				pLabel_2->setGeometry(0, 0, pWidget_2->width(), 20);
				pLabel_2->setText("Detector 2");
				pLabel_2->setAlignment(Qt::AlignCenter);
				pWidget_2->setWindowTitle(StrLabel_2);
				pScreenSplitter->addWidget(pWidget_2);
			}
			else                     // ̽����1��ͼ��ʵ��
			{
				// ��ʼ��
				pImageDisplay.reset(new ImageDisplay());
				pWidget_2 = new ImageWidget();
				pLabel_2 = new QLabel(pWidget_2);

				// ���ڲ�������
				pWidget_2->setWindowTitle(PairDetIns_2.first);
				pWidget_2->setGeometry(0, 0,
					pScreenSplitter->width() / 2,
					pScreenSplitter->height());
				pLabel_2->setText(PairDetIns_2.first);
				pLabel_2->setGeometry(0, 0, pWidget_2->width(), 20);
				pLabel_2->setAlignment(Qt::AlignCenter);

				// ͼ��ʵ��ת��ΪWidget
				iRet = pImageDisplay.get()->InstanceToDisplay(PairDetIns_2.second, pWidget_2);
				if (SUCCESS != iRet) return;

				// ���ش�����Splitter
				pScreenSplitter->addWidget(pWidget_2);
			}

			// ��1��̽��������Ϊѡ��
			SeletctDet(Detector_1);

			break;
		}

		// �б���ʾ
		case DisplayMode_ImageList:
		{
			// ɾ��֮ǰ�Ŀؼ�
			if (nullptr != pScreenMdiArea)
			{
				delete pScreenMdiArea;
				pScreenMdiArea = nullptr;
			}
			if (nullptr != pScreenSplitter)
			{
				pScreenSplitter->close();
				delete pScreenSplitter;
				pScreenSplitter = nullptr;
			}

			// ��ʼ��
			pScreenMdiArea = new QMdiArea(ui.centralWidget);
			
			// ��������
			pScreenMdiArea->setGeometry(0, 0,
				ui.centralWidget->width(),
				ui.centralWidget->height());
			pScreenMdiArea->setViewMode(QMdiArea::TabbedView);   // ����ΪTabҳ����ʾ
			pScreenMdiArea->setTabsClosable(true);               // ����ΪTabҳ����ʾ 	
			pScreenMdiArea->setMouseTracking(true);
			iMdiHeight = pScreenMdiArea->height();
			iMdiWidth = pScreenMdiArea->width();
			
			// ��ʾ
			pScreenMdiArea->show();

			// ��������ʾ����ͼ��
			for (int i = 0; i < MapImageIns.Size(); i++)
			{
				// ��ǰͼ��ʵ����ֵ
				PairCurrImageIns.first = MapImageIns.KeyAt(i);
				PairCurrImageIns.second = MapImageIns.ValueAt(i);
				
				// ��ʼ��
				pSubWidget = new ImageWidget();
				pImageDisplay.reset(new ImageDisplay());

				// ��������
				pSubWidget->setGeometry(0, 0, iMdiWidth, iMdiHeight - 25);
				pSubWidget->setWindowTitle(PairCurrImageIns.first);
				
				// ͼ��ʵ��ת��ΪWidget
				iRet = pImageDisplay.get()->InstanceToDisplay(PairCurrImageIns.second, pSubWidget);
				if (SUCCESS != iRet) return;

				// �����Ӵ���
				pScreenMdiArea->addSubWindow(pSubWidget);
				pSubWidget->show();
				pSubWidget->setWindowState(Qt::WindowMaximized);
				connect(pSubWidget, &ImageWidget::ImgWidgetClose, this, &ZCIPSMainFrame::DeleteInstance);
			}

			// �ź�-�ۺ���
			connect(pScreenMdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), 
				this, SLOT(MdiAreaSwitch(QMdiSubWindow*)));

			break;
		}
		
		case DisplayMode_None:
		default: 
		{
			break;
		}
			
	}

	return;
}

/**************************************************************
�������� UpdatePanel

���� ��  ����Panel��ʾ
		 �ø������ڳ���������������������Panel�ⲿ�Ķ�Panel�ĸ�����Ҫ

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::UpdatePanel()
{
	// ��������
	bool bRet;
	ProcessType m_PType;
	PanelTab m_PanelTab;
	ImageProcessTab m_ImageProcessTab;

	// �ж��Ƿ���ڵ�ǰͼ��ʵ��
	bRet = IsCurrImageInstanceValid();
	if (true != bRet) return;

	// �ж��Ƿ���ڵ�ǰͼ����
	bRet = PairCurrImageIns.second.get()->IsCurrProcessValid();

	// �����ڵ�ǰͼ������PanelӦ����ת����Ӧ��ҳ��
	// �������ڣ�����Ե�ǰ��ʾ��ҳ����и���
	if (true == bRet)
	{
		// ��ȡ��������
		m_PType = PairCurrImageIns.second.get()
			->pImageProcessIns.get()->PairCurrProcess.first;

		// ��תҳ��ѡ��
		switch (m_PType)
		{
			// �Ҷȱ任
			case ProcessType_GrayTrans:
			{
				// ȡ������ͼ��ʵ��
				bIsUpdateImageIns = false;
				
				// �л���ͼ����Tab
				SwitchPanelTab(PanelTab_ImagePro);

				// �л����Ҷȱ任Tab
				SwitchImageProcessTab(ImageProcessTab_GrayTrans);
				
				// ��������ͼ��ʵ��
				bIsUpdateImageIns = true;

				break;
			}
			
			// ͼ����ǿ 
			case ProcessType_Enhancement:
			{
				// ȡ������ͼ��ʵ��
				bIsUpdateImageIns = false;
				
				// �л���ͼ����Tab
				SwitchPanelTab(PanelTab_ImagePro);

				// �л���ͼ����ǿTab
				SwitchImageProcessTab(ImageProcessTab_Enhancement);
				
				// ��������ͼ��ʵ��
				bIsUpdateImageIns = true;

				break;
			}
			
			case ProcessType_None:
			default:
			{
				return;
			}
		}
	}
	else
	{
		// ��ȡ��ǰѡ��� Panel Tab
		m_PanelTab = (PanelTab)ui.PanelTabWidget->currentIndex();

		// Panel Tab
		switch (m_PanelTab)
		{
			// ����ҳ��Tab
			case PanelTab_Control:
			{
				
				
				break;
			}
			
			// ͼ����Tab
			case PanelTab_ImagePro:
			{
				// ��ȡ��ǰѡ��� Image Process Tab
				m_ImageProcessTab = (ImageProcessTab)ui.ImageProcessTabWidget->currentIndex();
				
				// ��ǰѡ���ͼ����ҳ��Tab
				switch (m_ImageProcessTab)
				{
					// �Ҷȱ任
					case ImageProcessTab_GrayTrans:
					{
						// �Ҷȱ任����
						GrayTransTabUpdate();

						// ��������
						MarcoTabUpdate();
						
						break;
					}
					
					// ͼ����ǿ
					case ImageProcessTab_Enhancement:
					{
						// ��������
						MarcoTabUpdate();
						
						break;
					}
					
					default:
					{
						// ��������
						MarcoTabUpdate();
						
						break;
					}	
				}

				break;
			}
				
			default:
			{
				break;
			}
				
		}
	}
	
	return;
}

/**************************************************************
�������� GetCurrImageInstance

���� ��  ��ȡ��ǰ��ʾ��ͼ��ʵ��

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::GetCurrImageInstance()
{
	// ��������
	bool bRet;									// ��������ֵ
	//QWidget* pSubWidget;						// �Ӵ���
	QWidget *pSubWidget;
	QSharedPointer<ImageInstance> pImageIns;    // ͼ��ʵ��ָ��
	
	// ������ֵ
	pImageIns.clear();
	pSubWidget = nullptr;

	// ��ʾģʽ�ж�
	// ��ȡQWidgetָ��
	switch (m_DisplayMode)
	{
		// ̽������ʾģʽ
		case DisplayMode_Detector:
		{
			switch (m_DetectorNo)
			{	
				// ̽����1��
				case Detector_1:
				{
					// �ж�̽����ʵ���Ƿ����
					// ���������˳�
					bRet = IsDetInstanceValid(m_DetectorNo);
					if (false == bRet) return;

					// ��ǰͼ��ʵ����ֵ
					PairCurrImageIns.first = PairDetIns_1.first;
					PairCurrImageIns.second = PairDetIns_1.second;

					break;
				}

				// ̽����2��
				case Detector_2:
				{
					// �ж�̽����ʵ���Ƿ����
					// ���������˳�
					bRet = IsDetInstanceValid(m_DetectorNo);
					if (false == bRet) return;

					// ��ǰͼ��ʵ����ֵ
					PairCurrImageIns.first = PairDetIns_2.first;
					PairCurrImageIns.second = PairDetIns_2.second;

					break;
				}
				default:
				{
					break;
				}	
			}
			
			break;
		}
		
		// ͼ���б���ʾģʽ
		case DisplayMode_ImageList:
		{
			// ��ȡ��ǰ���ڴ����ͼ��ָ��
			// ����ȡΪ��ָ�룬���˳�
			pSubWidget = pScreenMdiArea->activeSubWindow();
			if (nullptr == pSubWidget) return;
			
			// 
			pImageIns = MapImageIns.Find(pSubWidget->windowTitle());
			if (true == pImageIns.isNull())   // δ�ҵ��κδ���ͼ��ʵ��
			{
				PairCurrImageIns.first.clear();
				PairCurrImageIns.second.clear();
			}
			else							  // ��ǰ����ʵ����ֵ
			{
				PairCurrImageIns.first = pSubWidget->windowTitle();
				PairCurrImageIns.second = pImageIns;
			}

			break;
		}
		
		default:
		{
			break;
		}
		
	}

	return;
}

/**************************************************************
�������� IsCurrImageInstanceValid

���� ��  �жϵ�ǰ����ͼ��ʵ���Ƿ����

��ڲ��� ��

��������ֵ ��
	true ��		����
	false ��	������
****************************************************************/
bool ZCIPSMainFrame::IsCurrImageInstanceValid()
{
	if ((true == PairCurrImageIns.first.isEmpty())
		|| (true == PairCurrImageIns.second.isNull()))
	{
		return false;
	}
	else
	{
		return true;
	}
}

/**************************************************************
�������� IsCurrProValChanged

���� ��  ��ǰ�����Ƿ���ڲ����ı�

��ڲ��� ��

��������ֵ ��
	true ��		����
	false ��	������
****************************************************************/
bool ZCIPSMainFrame::IsCurrProValChanged()
{
	// ��������
	bool bRet;
	ProcessType PType;
	
	// ���ͻ�ȡ
	PType = PairCurrImageIns.second.get()
		->pImageProcessIns.get()->PairCurrProcess.first;

	// ���������ж�
	switch (PType)
	{
		// ͼ��任
		case ProcessType_GrayTrans:
		{
			// �Ҷȱ任ָ�븳ֵ
			GrayTransProcess *pGTPro = 
				(GrayTransProcess *)PairCurrImageIns.second.get()
					->pImageProcessIns.get()->PairCurrProcess.second;
			
			// �жϵ�ǰ�����Ƿ������Ч�Ĳ����޸�ֵ
			bRet = pGTPro->bIsValChanged();
			if (false == bRet) return false;
			else return true;

			break;
		}
		
		// ͼ����ǿ
		case ProcessType_Enhancement: break;
			
		// ����
		case ProcessType_None:
		default:
		{
			break;
		}
			
	}

	return true;
}

/**************************************************************
�������� IsDetInstanceValid

���� ��  �ж�̽����ͼ��ʵ���Ƿ����

��ڲ��� ��
	DetectorNo DetNo : ̽�������

��������ֵ ��
	true ��		����
	false ��	������
****************************************************************/
bool ZCIPSMainFrame::IsDetInstanceValid(DetectorNo DetNo)
{
	switch (DetNo)
	{
		// 1��̽����
		case Detector_1:
		{
			
			
			if ((true == PairDetIns_1.first.isEmpty())
				|| (true == PairDetIns_1.second.isNull()))
			{
				return false;
			}
			else
			{
				return true;
			}	
		}

		// 2��̽����
		case Detector_2:
		{
			if ((true == PairDetIns_2.first.isEmpty())
				|| (true == PairDetIns_2.second.isNull()))
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		
		default:
		{
			return false;
		}
	}
}

/**************************************************************
�������� InitialImgDisplay

���� ��  ��ʼ��ͼ����ʾ

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ZCIPSMainFrame::InitialImgDisplay()
{
	// ������ֵ
	pScreenSplitter = nullptr;
	pScreenMdiArea = nullptr;

	// ��ʾģʽ����
	m_DisplayMode = DisplayMode_None;

	// �л���ʾ
	DisplaySwitch();
}
/***********************************************************************/
/*************************** ���ݿ⴦�� start ************************/
/********************************************************************
��������DataBaseUIInit

�������壺���ݿ������Ʋ���ʼ��

��ڲ�����

��������ֵ��
********************************************************************/
void ZCIPSMainFrame::DataBaseUIInit()
{
	//��tableview��������
	ui.tableView->setSelectionMode(QAbstractItemView::SingleSelection);//��ѡ
	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//����Ϊѡ���׼
	ui.tableView->horizontalHeader()->setHighlightSections(false);
	ui.tableView->horizontalHeader()->setBackgroundRole(QPalette::Background);
	ui.tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(255,255,255)};");
	ui.tableView->setEditTriggers(QAbstractItemView::SelectedClicked);
	//���ݿ�����
	DBProcess.reset(new DataBaseProcess);
	if (!DBProcess.get()->ConnectDB())
	{
		ui.DisplyDBinfolabel->setText("���ݿ�����ʧ�ܣ�");
		return;
	}
	DBProcess.get()->sDBFilePath = "C:/Users/yyq/Desktop/tif";//��Ҫ�޸ĵ�·������·�������Լ�����
	//DBProcess.get()->InsertDBdata();
	ui.DisplyDBinfolabel->setText("���ݿ����ӳɹ���");
	connect(ui.DBcheckButton, SIGNAL(clicked()), this, SLOT(SearchDataBase()));
	connect(ui.UpdateDBButton, SIGNAL(clicked()), this, SLOT(UpdateDataBase()));
	connect(ui.SubmitButton, SIGNAL(clicked()), this, SLOT(SubmmitDataBase()));
	connect(ui.DeleteRecButton, SIGNAL(clicked()), this, SLOT(DeleteDataBaseEntry()));
}
/****************************************************************
��������UpdateDBdisplyInfo

�������壺���������������ʾ��Ϣ��

��ڲ�����

��������ֵ��
********************************************************************/
void ZCIPSMainFrame::UpdateDBdisplyInfo(const QModelIndex index)
{
	QModelIndex index1 = ui.tableView->currentIndex();
	if (index1.isValid())
	{
		int rownum = ui.tableView->currentIndex().row();
		ui.DisplyDBinfolabel->setText("ѡ�м�¼" + DBProcess.get()->model->data(DBProcess.get()->model->index(rownum, 0)).toString() + "!");
		ui.Filenamelabel->setText(DBProcess.get()->model->data(DBProcess.get()->model->index(rownum, 0)).toString());
		ui.Inspecdatelabel->setText(DBProcess.get()->model->data(DBProcess.get()->model->index(rownum, 1)).toString());
		ui.Inspecpersonlabel->setText(DBProcess.get()->model->data(DBProcess.get()->model->index(rownum, 2)).toString());
		ui.Spiecenamelabel->setText(DBProcess.get()->model->data(DBProcess.get()->model->index(rownum, 3)).toString());
		ui.Fileexistlabel->setText(DBProcess.get()->model->data(DBProcess.get()->model->index(rownum, 4)).toString());
	}
}
/***************************************************************
��������SearchDataBase

�������壺���ݼ���������ѯ���ݿ�

��ڲ�����

��������ֵ��
******************************************************************/
void ZCIPSMainFrame::SearchDataBase()
{
	DBProcess.get()->model->setTable("ImgTable");
	QString strSqlqurry = "";
	QString temppstr;
	if (ui.InspectcheckBox->isChecked())
	{
		temppstr = "������� between '" + ui.dateEdit1->date().toString("yyyy-MM-dd") + "' and '" + ui.dateEdit2->date().toString("yyyy-MM-dd") + "'";
		strSqlqurry = strSqlqurry + temppstr;
	}
	if (ui.InspPersonEdit->text() != "")
	{
		if (strSqlqurry == "")
		{
			temppstr = " �����Ա like '" + ui.InspPersonEdit->text() + "'";
		}
		else
		{
			temppstr = "and �����Ա like '" + ui.InspPersonEdit->text() + "'";
		}
		strSqlqurry = strSqlqurry + temppstr;

	}
	if (ui.SpieceNameEdit->text() != "")
	{
		if (strSqlqurry == "")
		{
			temppstr = "�������� like '" + ui.SpieceNameEdit->text() + "'";
		}
		else
		{
			temppstr = "and �������� like '" + ui.SpieceNameEdit->text() + "'";
		}
		strSqlqurry = strSqlqurry + temppstr;
	}
	DBProcess.get()->model->setFilter(strSqlqurry);
	DBProcess.get()->model->select();
	ui.DisplyDBinfolabel->setText("���ҵ�" + QString::number(DBProcess.get()->model->rowCount(), 10) + "�����������ļ�¼��");
	ui.tableView->setModel(DBProcess.get()->model);
	ui.tableView->resizeColumnsToContents();
	//tableview������setmodel�Ժ���ܽ����źŲ�����
	connect(ui.tableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(UpdateDBdisplyInfo(const QModelIndex &)));
	connect(ui.tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(OpenSelectedFile(const QModelIndex &)));
	//ÿ�������¼������ݿ��Ժ󣬽���Ϣ��ʾ���ÿ�
	ui.Filenamelabel->setText("");
	ui.Inspecdatelabel->setText("");
	ui.Inspecpersonlabel->setText("");
	ui.Spiecenamelabel->setText("");
	ui.Fileexistlabel->setText("");
}
/***********************************************************************
��������UpdateDataBase

�������壺�������ݿ�����

��ڲ�����

��������ֵ��
************************************************************************/
void ZCIPSMainFrame::UpdateDataBase()
{
	ui.DisplyDBinfolabel->setText("���ݿ����ڸ��£�");
	DBProcess.get()->UpdateDBdata();
	DBProcess.get()->model->setTable("ImgTable");
	DBProcess.get()->model->select();
	ui.tableView->setModel(DBProcess.get()->model);
	ui.tableView->resizeColumnsToContents();
	ui.DisplyDBinfolabel->setText("���ݿ������ϣ�");
}
/********************************************************************
��������DeleteDataBaseEntry()

�������壺ɾ�����ݿ��е�ǰѡ����

��ڲ�����

��������ֵ��
***********************************************************************/
void ZCIPSMainFrame::DeleteDataBaseEntry()
{
	int currentrow = ui.tableView->currentIndex().row();
	DBProcess.get()->DeleteDBdata(currentrow);
	//ɾ����¼�������ʾ
	ui.DisplyDBinfolabel->setText("ɾ����¼�ɹ���");
	ui.Filenamelabel->setText("");
	ui.Inspecdatelabel->setText("");
	ui.Inspecpersonlabel->setText("");
	ui.Spiecenamelabel->setText("");
	ui.Fileexistlabel->setText("");
}
/**********************************************************************
��������SubmmitDataBase

�������壺�ύ�޸ĵ����ݿ�

��ڲ�����

��������ֵ��
*************************************************************************/
void ZCIPSMainFrame::SubmmitDataBase()
{
	if (DBProcess.get()->SubmitDBdata())
	{
		ui.DisplyDBinfolabel->setText("�ύ�޸ĵ����ݿ�ɹ���");
	}
	else
	{
		ui.DisplyDBinfolabel->setText("�ύ�޸ĵ����ݿ�ʧ�ܣ�");
	}
}
/***********************************************************************
��������OpenSelectedFile

�������壺��ѡ�е����ݿ��е��ļ�

��ڲ�����QModelIndex index  ѡ�еļ�¼���б��е�λ��

��������ֵ��
************************************************************************/
void ZCIPSMainFrame::OpenSelectedFile(const QModelIndex index)
{
	QModelIndex index1 = ui.tableView->currentIndex();
	if (index1.isValid())
	{
		int rownum = index1.row();
		QString StrFileName;						// �ļ�·��ȫ��
		// ��ȡ��ǰѡ�м�¼���ļ�·��ȫ��
		StrFileName = DBProcess.get()->model->data(DBProcess.get()->model->index(rownum, 0)).toString();
		// ��������
		int iRet;									// ��������ֵ
		int iHeight;
		int iWidth;
		QSharedPointer<ImageInstance> pImageIns;    // ͼ��ʵ��
		/*
		if (pScreenSplitter != NULL)
		{
			pScreenSplitter->close();
			delete pScreenSplitter;
			pScreenSplitter = NULL;
		}
		if (pScreenMdiArea == NULL)
		{
			pScreenMdiArea = new QMdiArea;
			this->setCentralWidget(pScreenMdiArea);
			pScreenMdiArea->setGeometry(0, 0, MdiAreaWidth, MdiAreaHeight);
			pScreenMdiArea->setMouseTracking(true);
			pScreenMdiArea->setViewMode(QMdiArea::TabbedView);
			pScreenMdiArea->setTabsClosable(true);

		}*/
		// ͼ��ʵ������
		pImageIns.reset(new ImageInstance);

		// ͼ��ߴ���ؼ��ߴ�����
		//pImageIns.get()->ResizeWidget(ui.ImageMdiArea->width(), ui.ImageMdiArea->height());
		//int ww = pScreenMdiArea->width();
		//int hh = pScreenMdiArea->height();
		//pImageIns.get()->ResizeWidget(pScreenMdiArea->width(), pScreenMdiArea->height());
		// ͼ��ʵ������

		iRet = pImageIns.get()->LoadFromFile(StrFileName);
		if (SUCCESS != iRet) return;

		// ������ͼ��ʵ���б�
		MapImageIns.Insert(StrFileName, pImageIns);

		// �л���ʾģʽ
		m_DisplayMode = DisplayMode_ImageList;
		DisplaySwitch();

		// ��ȡ��ǰ��ͼ��
		GetCurrImageInstance();




		// ������������
		pAreaSelect.clear();

		// ����Panel����
		UpdatePanel();
		ui.DisplyDBinfolabel->setText("ͼ��򿪳ɹ���");

	}
}
/********************************************** ���ݿ⴦�� end ***************************************************/

/****************************************** ͼ���� start *******************************/
/***************************************************************
�������ƣ�ItemToImageMark

�������壺��ͼԪת��Ϊ�����

��ڲ�����QGraphicsItem*item    ͼԪ

��������ֵ��MarksBase*    �����
*****************************************************************/
MarksBase*ZCIPSMainFrame::ItemToImageMark(QGraphicsItem*item)
{
	MarksBase*pTempMark = new MarksBase;
	switch (item->type())
	{
	case QGraphicsRectItem::Type:
	{
		MarkRect*ptemrect = new MarkRect;
		ptemrect->type = MarkType_rect;
		ptemrect->tooltip = item->toolTip();
		ptemrect->rect = qgraphicsitem_cast<QGraphicsRectItem*>(item)->rect();
		ptemrect->pen = qgraphicsitem_cast<QGraphicsRectItem*>(item)->pen();
		pTempMark = (MarksBase*)ptemrect;
		pTempMark->tooltip = item->toolTip();
		pTempMark->type = MarkType_rect;
	}
	break;
	case QGraphicsEllipseItem::Type:
	{
		MarkEllipse*ptemellipse = new MarkEllipse;
		ptemellipse->type = MarkType_ellipse;
		ptemellipse->tooltip = item->toolTip();
		ptemellipse->rect = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)->rect();
		ptemellipse->pen = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)->pen();
		pTempMark = (MarksBase*)ptemellipse;
		pTempMark->tooltip = item->toolTip();
		pTempMark->type = MarkType_ellipse;
	}
	break;
	case QGraphicsPolygonItem::Type:
	{
		MarkPolygon*ptempolygon = new MarkPolygon;
		ptempolygon->type = MarkType_polygon;
		ptempolygon->tooltip = item->toolTip();
		ptempolygon->polygon = qgraphicsitem_cast<QGraphicsPolygonItem*>(item)->polygon();
		ptempolygon->pen = qgraphicsitem_cast<QGraphicsPolygonItem*>(item)->pen();
		pTempMark = (MarksBase*)ptempolygon;
		pTempMark->tooltip = item->toolTip();
		pTempMark->type = MarkType_polygon;
	}
	break;
	case QImageRuler::Type:
	{
		MarkRuler*ptemruler = new MarkRuler;
		ptemruler->type = MarkType_Ruler;
		ptemruler->tooltip = item->toolTip();
		ptemruler->StrartPoint = qgraphicsitem_cast<QImageRuler*>(item)->StartPoint;
		ptemruler->EndPoint = qgraphicsitem_cast<QImageRuler*>(item)->EndPoint;
		ptemruler->fHPixPropotion = qgraphicsitem_cast<QImageRuler*>(item)->fHPixProportion;
		ptemruler->fWPixPropotion = qgraphicsitem_cast<QImageRuler*>(item)->fVPixProportion;
		ptemruler->pen = qgraphicsitem_cast<QImageRuler*>(item)->pen();
		pTempMark = (MarksBase*)ptemruler;
		pTempMark->tooltip = item->toolTip();
		pTempMark->type = MarkType_Ruler;
	}
	break;
	default:
		break;
	}
	return pTempMark;
}
/**********************************************************************
�������ƣ�MarklistAdd

�������壺��ͼԪ��ӽ�����б�

��ڲ�����QGraphicsItem*item     ͼԪ

��������ֵ��
**********************************************************************/
void ZCIPSMainFrame::MarklistAdd(QGraphicsItem*item)
{
	MarksBase*pTempMark = ItemToImageMark(item);
	PairCurrImageIns.second->pImageMarks->MarkListAppend(pTempMark);
	PairCurrImageIns.second->mouseType = Mouse_Default;
}
/**********************************************************************
�������ƣ�MarklistEdit

�������壺�༭����б��е�Ԫ��

��ڲ�����QGraphicsItem*item     ͼԪ

��������ֵ��
**********************************************************************/
void ZCIPSMainFrame::MarklistEdit(QGraphicsItem*item)
{
	MarksBase*pTempMark = ItemToImageMark(item);
	PairCurrImageIns.second->pImageMarks->MarkListEdit(pTempMark);
}
/*********************************************************************
�������ƣ�GetActWinandCurrentIns

�������壺��õ�ǰactive���ں͵�ǰʵ��

��ڲ�����

��������ֵ��
***********************************************************************/
void ZCIPSMainFrame::GetActWinandCurrentIns()
{
	QMdiSubWindow* pSubWidget;
	ImageScene *pImageScene;
	QGraphicsView *pImageView;
	pSubWidget = pScreenMdiArea->activeSubWindow();
	if (pSubWidget == NULL)return;
	pImageView = pSubWidget->findChild<QGraphicsView *>(QStringLiteral("ImageView"));
	if (pImageView != nullptr)
	{
		pImageScene = (ImageScene*)pImageView->scene();
		pImageScene->mouseType = PairCurrImageIns.second->mouseType;
		if (pImageScene->mouseType == Mouse_Default)
		{
			for (int i = 0; i < pImageScene->selectedItems().count(); i++)
			{
				pImageScene->selectedItems().at(i)->setSelected(false);
			}
		}
		connect(pImageScene, &ImageScene::AddNewItem, this, &ZCIPSMainFrame::MarklistAdd);
		connect(pImageScene, &ImageScene::EditItems, this, &ZCIPSMainFrame::MarklistEdit);
	}
}
/********************************************************************
�������ƣ�MarkChangeToRect

�������壺��������ı������ת��Ϊ����

��ڲ�����

��������ֵ��
*******************************************************************/
void ZCIPSMainFrame::MarkChangeToRect()
{
	// �ж��Ƿ���ڵ�ǰͼ��ʵ��
	bool bRet;
	bRet = IsCurrImageInstanceValid();
	if (true != bRet) return;
	PairCurrImageIns.second->mouseType = Mouse_Rect;
	GetActWinandCurrentIns();
}
/*******************************************************************
�������ƣ�MarkCHangeToEllipse

�������壺����ǰ���������ת��Ϊ��Բ

��ڲ�����

��������ֵ��
*******************************************************************/
void ZCIPSMainFrame::MarkCHangeToEllipse()
{
	// �ж��Ƿ���ڵ�ǰͼ��ʵ��
	bool bRet;
	bRet = IsCurrImageInstanceValid();
	if (true != bRet) return;
	PairCurrImageIns.second->mouseType = Mouse_Ellipse;
	GetActWinandCurrentIns();
}
/******************************************************************
�������ƣ�MarkChangeToPolygon

�������壺����ǰ���������ת��Ϊ�����

��ڲ�����

��������ֵ��
*****************************************************************/
void ZCIPSMainFrame::MarkChangeToPolygon()
{
	// �ж��Ƿ���ڵ�ǰͼ��ʵ��
	bool bRet;
	bRet = IsCurrImageInstanceValid();
	if (true != bRet) return;
	PairCurrImageIns.second->mouseType = Mouse_Polygon;
	GetActWinandCurrentIns();
}
/*********************************************************************
�������ƣ�MarkCancelMouseSelect

�������壺���������ת��Ϊ��

��ڲ�����

��������ֵ��
***********************************************************************/
void ZCIPSMainFrame::MarkCancelMouseSelect()
{
	// �ж��Ƿ���ڵ�ǰͼ��ʵ��
	bool bRet;
	bRet = IsCurrImageInstanceValid();
	if (true != bRet) return;
	PairCurrImageIns.second->mouseType = Mouse_Default;
	GetActWinandCurrentIns();
}
/*********************************************************************
�������ƣ�MarkChangeToRuler

�������壺�����������ת��Ϊ���Ȳ���

��ڲ�����

��������ֵ��
*******************************************************************/
void ZCIPSMainFrame::MarkChangeToRuler()
{
	// �ж��Ƿ���ڵ�ǰͼ��ʵ��
	bool bRet;
	bRet = IsCurrImageInstanceValid();
	if (true != bRet) return;
	PairCurrImageIns.second->mouseType = Mouse_Ruler;
	GetActWinandCurrentIns();
}
/****************************************** ͼ���� end ************************************/