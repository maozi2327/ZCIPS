#include "stdafx.h"
#include "conescaninterface.h"
#include "controllerinterface.h"
#include "imageprocess.h"
#include "../../Public/util/functions.h"

ConeScanInterface::ConeScanInterface(Panel* _panel, ControllerInterface* _controller, PanelImageProcess* _imageProcess):
	d_panel(_panel), d_controller(_controller), d_imageProcess(_imageProcess),
	d_imageProcessSleep(300), d_graduationCount(0)
{
	d_width = d_panel->getPanelSize().first;
	d_height = d_panel->getPanelSize().second;
}

ConeScanInterface::~ConeScanInterface()
{
	
}

bool ConeScanInterface::saveFile(unsigned short * _image)
{
	QString index;
	index.sprintf("%4d", int(d_graduationCount));
	auto completeOrgFileName = d_fileFolder + d_fileName + "org/" + index;
	d_imageProcess->saveSingleBitmapDataToFile(_image, completeOrgFileName + d_fileName, d_height, d_width);
	emit imageAcquiredSignal(_image, d_width, d_height);
	free(_image);
	emit scanProgressSignal(float(d_graduationCount) / (d_graduation * d_framesPerGraduation));

	//if (d_orgFlag && !d_averageFlag)
	//	d_imageProcess->saveMultiBitmapDataToFile(_image, completeOrgFileName + d_fileName, d_framesPerGraduation, d_height, d_width);
	//else if (d_orgFlag && !d_averageFlag)
	//	d_imageProcess->saveSingleBitmapDataToFile(_image, completeOrgFileName + d_fileName, d_height, d_width);

	//auto completeImageFileName = d_fileFolder + d_fileName + "ct/" + index;

	//if (d_bkgFlag && !d_airFlag && !d_defectFlag)
	//	d_imageProcess->bkgCorrectDataToFile(_image, completeImageFileName + d_fileName, d_height, d_width);
	//else if (d_bkgFlag && d_airFlag && !d_defectFlag)
	//	d_imageProcess->airCorrectDataToFile(_image, completeImageFileName + d_fileName, d_height, d_width);
	//else if (d_bkgFlag && d_airFlag && d_defectFlag)
	//	d_imageProcess->defectCorrectDataToFile(_image, completeImageFileName + d_fileName, d_height, d_width);

	return false;
}

bool ConeScanInterface::checkMemory()
{
	size_t allImageSize = size_t(d_framesPerGraduation) * d_graduation *d_width * d_height * 
		sizeof(unsigned short) / (1024 * 1024);

	if (d_isSaveOrg)
		allImageSize *= 2;
	
	return allImageSize < getDiskFreeSpace(d_fileFolder);
}

void ConeScanInterface::frameProcessCallback(unsigned short * _image)
{
	d_imageList.push(_image);
	//d_con.notify_one();
}

void ConeScanInterface::imageProcessThread()
{
	while (d_deadThreadRun)
	{
		unsigned short* imageData = nullptr;

		if (d_imageList.pop(imageData))
		{
			++d_graduationCount;
			saveFile(imageData);
		}
		//if (++d_frameCount == d_framesPerGraduation)
		//{
		//	if (d_frameCount == 1)
		//	{
		//		d_imageList.pop(imageData);
		//	}
		//	else
		//	{
		//		char* imageMem = (char*)malloc(d_frameSize);
		//		int count = 0;
		//			
		//		while (count++ != d_framesPerGraduation)
		//		{
		//			unsigned short* mem;
		//			d_imageList.pop(mem);
		//			memcpy(imageMem + d_frameSize * count, mem, d_frameSize);
		//			free(mem);
		//		}
		//			
		//		imageData = (unsigned short*)imageMem;
		//	}
		//	d_frameCount = 0;
		//}
	}
}

bool ConeScanInterface::canScan()
{
	QString str;
	
	if (!checkMemory())
	{
		LOG_ERROR("硬盘空间不足");
		return false;
	}

	if (!d_controller->checkReadyToScan())
	{
		LOG_ERROR("控制器未就绪");
		return false;
	}

	if (!d_panel->getReady())
	{
		LOG_ERROR("探测器未就绪");
		return false;
	}

	return true;
}

bool ConeScanInterface::loadBkgData()
{
	if(!d_imageProcess->loadBkgData(QString("bkg.tif")))
	{
		LOG_ERROR("锥束扫描加载空气文件失败！");
		return false;
	}

	return true;
}

bool ConeScanInterface::writeParameterFile()
{
	QFile paraFile;
	QString parameterFileName = d_fileFolder + d_fileName + "\\" + d_parameterFileName;
	paraFile.setFileName(parameterFileName);

	if (!paraFile.open(QIODevice::ReadWrite))
	{
		LOG_ERROR("新建参数文件失败,扫描工程号：" + d_fileName);
		return false;
	}

	makeParameterText();
	QString text;

	for (auto& line : d_parameterText)
		text += line;

	paraFile.write(text.toLocal8Bit());
	paraFile.close();
	return false;
}

bool ConeScanInterface::makeParameterText()
{
	d_parameterText.resize(300);
	size_t index = 0;
	d_parameterText[index++] = ";--------------------------------------------;\n";
	d_parameterText[index++] = ";  扫描参数\n";
	d_parameterText[index++] = ";--------------------------------------------;\n";
	d_parameterText[index++] = ";--------------------------------------------;\n";

	d_parameterText[index++] = ";  探测器参数\n";
	d_parameterText[index++] = ";--------------------------------------------;\n";
	d_parameterText[index++] = "[DETECTORS]\n";
	d_parameterText[index++].sprintf("Width=%d\t\t\t;探测器横向单元数量\n", d_width);
	d_parameterText[index++].sprintf("Height=%d\t\t\t;探测器纵向单元数量\n", d_height);
	d_parameterText[index++].sprintf("UnitSize=%.6f\t\t\t;探测器单元宽度（mm）\n", d_panel->getPixelSize());
	d_parameterText[index++] = "Binning=1;Binning方式:1-1X1,2-2X2,3-4X4,4-1X2,5-,6-1X4\n";
	d_parameterText[index++].sprintf("SampleTime = %.3f\t\t\t; 采样积分时间（秒）\n", float(d_panel->getSampleTime()) / 1000);
	d_parameterText[index++].sprintf("FrameCnt=%d\t\t\t;单分度采样帧数\n", d_framesPerGraduation);
	d_parameterText[index++] = "DataType=INT16\t\t\t;采集数据类型，INT16，INT32，FLOAT\n\n";

	d_parameterText[index++] = (";--------------------------------------------;\n");
	d_parameterText[index++] = (";  扫描几何参数\n");
	d_parameterText[index++] = (";  以转轴为z轴，垂直z轴的射线为y轴（方向指向射线源），建立全局坐标系；\n");
	d_parameterText[index++] = (";  探测器水平方向为u轴，垂直方向为v轴，建立局部坐标系；\n");
	d_parameterText[index++] = (";  局部坐标系绕全局系z轴旋转。\n");
	d_parameterText[index++] = (";--------------------------------------------;\n");
	d_parameterText[index++] = ("[GEOMETRY]\n");
	d_parameterText[index++].sprintf("SOD=%.1f\t\t\t;射线源焦点到旋转轴的实际距离（mm）\n", AxisPosEnum::objRadial);
	d_parameterText[index++].sprintf("SDD=%.1f\t\t\t;射线源焦点到探测器的实际距离（mm）\n", d_controller->readAxisPostion(AxisPosEnum::detRadial));
	d_parameterText[index++].sprintf("thetaY=%.2f\t\t\t;探测器绕y轴的旋转角度（度）\n", 0.0f);
	d_parameterText[index++].sprintf("thetaX=%.2f\t\t\t;探测器绕x轴的旋转角度（度）\n", 0.0f);
	d_parameterText[index++].sprintf("thetaZ=%.2f\t\t\t;探测器绕z轴的旋转角度（度）\n", 0.0f);
	d_parameterText[index++].sprintf("uo_mm=%.1f\t\t\t;中心探测器水平坐标（mm）\n", d_panel->getPixelSize() * d_width / 2);
	d_parameterText[index++].sprintf("vo_mm=%.1f\t\t\t;中心探测器垂直坐标（mm）\n", d_panel->getPixelSize() * d_height / 2);
	d_parameterText[index++].sprintf("graduation=%d\t\t\t;分度数\n", d_graduation);
	d_parameterText[index++].sprintf("Angle00000=%d\t\t\t;0号投影的角度\n", d_orientInc);
	d_parameterText[index++].sprintf("…\t\t\t\t; 1-%04u号投影的角度\n", d_graduation);
	d_parameterText[index++].sprintf("Angle%04u=%d\t\t\t;%04u号投影的角度\n\n",
		d_graduation, d_orientInc + 360, d_graduation);

	d_parameterText[index++] = (";---------------------\n");
	d_parameterText[index++] = (";  重建图像参数\n");
	d_parameterText[index++] = (";---------------------\n");
	d_parameterText[index++] = ("[IMAGE PARAMETER]\n");
	d_parameterText[index++].sprintf("Width=%d\t\t\t;重建图像的宽度或直径（像素）\n", 800);
	d_parameterText[index++].sprintf("StartLayer=%d\t\t\t;重建图像的起始层（像素）\n", 0);
	d_parameterText[index++].sprintf("Height=%d\t\t\t;重建图像的高度或层数（像素）\n", 800);
	d_parameterText[index++].sprintf("SkipGraduation=%d\t\t\t;分度跳跃数\n", 1);
	d_parameterText[index++].sprintf("SkipHorizontal=%d\t\t\t;探测器水平方向跳跃数\n", 0);
	d_parameterText[index++].sprintf("SkipVertical =%d\t\t\t;探测器垂直方向跳跃数\n", 0);
	d_parameterText[index++].sprintf("rotAngle=%d\t\t\t;重建起始角度（°）\n", 0);
	d_parameterText[index++].sprintf("FirstSection=%d\t\t\t;\n", 100);
	d_parameterText[index++].sprintf("LastSection=%d\t\t\t;\n", 800);
	d_parameterText[index++].sprintf("Filename=\n");
	d_parameterText[index++].sprintf("Image Directory=%s\t\t\t;\n", "C:\\");
	d_parameterText[index++].sprintf("Sigma=%.6f\t\t\t;\n", 2.0);
	return true;
}

bool ConeScanInterface::loadDefectData()
{
	return d_imageProcess->loadDefectData(QString("defect.tif"));
}
void ConeScanInterface::setFileName(QString& _fileFolder, QString & _name)
{
	d_fileFolder = _fileFolder;
	d_fileName = _name;
}

void ConeScanInterface::setDisposeFlag(bool _bkgFlag, bool _airFlag,
	bool _defectFlag, bool _averageFlag)
{
	d_bkgFlag = _bkgFlag;
	d_airFlag = _airFlag;
	d_defectFlag = _defectFlag;
	d_averageFlag = _averageFlag;
}

bool ConeScanInterface::stopScan()
{
	d_imageProcessThreadFlag = false;
	return true;
}

bool ConeScanInterface::beginScan(int _graduation, int _framesPerGraduation, int _posTime, int _cycleTime,
	unsigned short _gainFactor, float _orientInc)
{
	d_graduationCount = 0;
	d_graduation = _graduation;
	d_framesPerGraduation = _framesPerGraduation;
	d_posTime = _posTime;
	d_orientInc = _orientInc;

	//if (!canScan())
	//	return false;

	//if(!writeParameterFile())
	//	return false;
	//
	//if (!loadBkgData())
	//	return false;

	//if (!loadAirData())
	//	return false;
	d_deadThreadRun = true;
	sendCmdToController();
	d_scanThread.reset(new Thread(std::bind(&ConeScanInterface::scanThread, this), std::ref(d_deadThreadRun)));
	d_scanThread->detach();
	d_imageProcessThread.reset(new Thread(std::bind(&ConeScanInterface::imageProcessThread, this), std::ref(d_deadThreadRun)));
	d_imageProcessThread->detach();
	std::function<void(unsigned short *)> frameCallback = std::bind(
		&ConeScanInterface::frameProcessCallback, this, std::placeholders::_1);
	d_panel->beginExTriggerAcquire(frameCallback, _cycleTime, _gainFactor);
	return true;
}

void ConeScanInterface::getScanProgress(int & _thisRound, int & _allProgress, QString & imagesCollectedAndSpaceOccupied)
{
	_thisRound = 100 * d_graduationCount / d_graduation;
	_allProgress = 100 * d_graduationCount / d_graduation;
	imagesCollectedAndSpaceOccupied.sprintf("已经采集%d幅，占用硬盘空间%dm", int(d_graduationCount),
		size_t(d_graduation) * d_frameSize * d_framesPerGraduation / (1024 * 1024));
}
