#include "stdafx.h"
#include "conescaninterface.h"
#include "controllerinterface.h"
#include "imageprocess.h"
#include "../../Public/util/functions.h"
#include "../PanelDetector/panel.h"


QString ConeScanInterface::d_parameterFileName{ QString::fromLocal8Bit("ScanParameter.txt") };

ConeScanInterface::ConeScanInterface(Panel* _panel, ControllerInterface* _controller, PanelImageProcess* _imageProcess, bool _bkgTuneFlag,
	bool _airTuneFlag, bool _defectTuneFlag, PanDetData _pandetData)
	: d_panel(_panel), d_controller(_controller), d_imageProcess(_imageProcess),d_imageProcessSleep(300), d_graduationCount(0)
	, d_airTuneFlag(_airTuneFlag), d_bkgTuneFlag(_bkgTuneFlag), d_defectTuneFlag(_defectTuneFlag), d_round(1), d_frameSize(_panel->getFrameSize())
	, d_width(_panel->getPanelSize().first), d_height(_panel->getPanelSize().second)
	, d_pandetData(_pandetData)
{

}

ConeScanInterface::~ConeScanInterface()
{
	//TODO_DJ：扫描停止后重新这里会析构，析构后Thread里面stopThread会挂，mutex
}

bool ConeScanInterface::saveFile(unsigned short * _image)
{
	QString orgFileName, tunedFileName;
	createFileName(orgFileName, tunedFileName);

	if (d_saveOrg)
		d_imageProcess->saveSingleBitmapDataToFile(_image, orgFileName, d_height, d_width);

	if (d_defectTuneFlag)
		d_imageProcess->bkgAirDefectTuneDataToFile(_image, tunedFileName, d_height, d_width);
	else if (d_airTuneFlag)
		d_imageProcess->bkgAirTuneDataToFile(_image, orgFileName, d_height, d_width);
	else
	{
		if (d_saveOrg)
			QFile::copy(orgFileName, tunedFileName);
		else
			d_imageProcess->saveSingleBitmapDataToFile(_image, tunedFileName, d_height, d_width);
	}

	return false;
}

void ConeScanInterface::createFileName(QString& _orgFileName, QString& _tunedFileName)
{
	if (d_graduationCount == 0)
	{
		QString orgPath = d_orgName.left(d_orgName.lastIndexOf('/') + 1);
		QString timeName = getTimeWithUnderline();
		d_pureDirectoryName = timeName + d_orgName.right(d_orgName.length() - d_orgName.lastIndexOf('_')) + QString::fromLocal8Bit("/");
		d_orgName = orgPath + d_pureDirectoryName;
		QDir dir;
		dir.mkpath(d_orgName);
		makeParameterText();
		writeParameterFile(d_orgName);
		dir.mkpath(d_tunedFilePath + d_pureDirectoryName);
		writeParameterFile(d_tunedFilePath + d_pureDirectoryName);
	}

	QString index;
	index.sprintf("%04d", int(d_graduationCount));
	_orgFileName = d_orgName + index + QString::fromLocal8Bit(".tif");
	_tunedFileName = d_tunedFilePath + d_pureDirectoryName + index + QString::fromLocal8Bit(".tif");
}

bool ConeScanInterface::checkMemory()
{
	size_t allImageSize = size_t(d_framesPerGraduation) * d_graduation *d_width * d_height * 
		sizeof(unsigned short) / (1024 * 1024);

	if (d_isSaveOrg)
		allImageSize *= 2;
	
	return allImageSize < getDiskFreeSpace(d_fileFolder);
}

bool ConeScanInterface::setCommonScanParameter(int _graduation, int _framesPerGraduation, int _round, int _posTime, int _cycleTime, unsigned short _gainFactor, float _orientInc, float _slicePos, float _sod, float _sdd)
{
	d_graduationCount = 0;
	d_graduation = _graduation;
	d_framesPerGraduation = _framesPerGraduation;
	d_posTime = _posTime;
	d_orientInc = _orientInc;
	d_slicePos = _slicePos;
	d_round = _round;
	d_imageList.clear();
	d_sod = _sod;
	d_sdd = _sdd;
	d_scanProc = 0;
	d_cycleTime = _cycleTime;
	d_gainFactor = _gainFactor;
	d_scanTime = d_posTime * d_framesPerGraduation * d_graduation * d_round / 1000;
	return true;
}

void ConeScanInterface::detachScanProcessThread()
{
	d_scanThreadRun = true;
	d_scanThread.reset(new Thread(std::bind(&ConeScanInterface::scanThread, this), std::ref(d_scanThreadRun)));
	d_scanThread->detach();
	d_imageProcessThread.reset(new Thread(std::bind(&ConeScanInterface::imageProcessThread, this), std::ref(d_imageProcessThreadRun)));
	d_imageProcessThread->detach();
}

void ConeScanInterface::frameProcessCallback(unsigned short * _image)
{
	unsigned short* memory = new unsigned short[d_width * d_height];
	memcpy(memory, _image, sizeof(unsigned short) * d_width * d_height);
	d_imageList.push(memory);
}

void ConeScanInterface::imageProcessThread()
{
	while (d_imageProcessThreadRun)
	{
		unsigned short* memory;

		if (d_imageList.pop(memory))
		{
			if (d_graduationCount < d_round * d_graduation * d_framesPerGraduation)
			{
				//TODO_DJ：添加旋转或者对称标志到配置文件
				d_imageProcess->mirrorYDataToData(memory, d_height, d_width);
				saveFile(memory);
				int timeRemain = (d_round * d_graduation * d_framesPerGraduation - (d_graduationCount + 1)) * d_posTime / 1000 / 60 + 1;
				QString message = makeFormatQString("已经采集%d幅，剩余时间：%d分", d_graduationCount + 1, timeRemain);
				emit scanProgressSignal(d_graduationCount + 1, d_graduation * d_framesPerGraduation, d_graduationCount + 1,
					d_round * d_graduation * d_framesPerGraduation, message);
			}

			delete[] memory;
			++d_graduationCount;
			//TODO_DJ：round
		}
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

bool ConeScanInterface::writeParameterFile(const QString& _path)
{
	QFile paraFile;
	QString parameterFileName = _path + d_parameterFileName;
	paraFile.setFileName(parameterFileName);

	if (!paraFile.open(QIODevice::ReadWrite))
	{
		LOG_ERROR("新建参数文件失败,扫描工程号：" + d_fileName);
		return false;
	}

	QString text;

	for (auto& line : d_parameterText)
		text += line;

	paraFile.write(text.toLocal8Bit());
	paraFile.close();
	return false;
}

bool ConeScanInterface::makeParameterText()
{
	size_t index = 0;
	d_parameterText = { 
		QString::fromLocal8Bit(";--------------------------------------------;\n"), 
		QString::fromLocal8Bit(";  扫描参数\n"),
		QString::fromLocal8Bit(";--------------------------------------------;\n"),
		QString::fromLocal8Bit(";  探测器参数\n"),
		QString::fromLocal8Bit("[DETECTORS]\n"),
		makeFormatQString("Width=%d\t\t\t;探测器横向单元数量\n", d_width), 
		makeFormatQString("Height=%d\t\t\t;探测器纵向单元数量\n", d_height),
		makeFormatQString("UnitSize=%.6f\t\t\t;探测器单元宽度（mm）\n", d_pandetData.pixelSize),
		makeFormatQString("Binning=1;Binning方式:1-1X1,2-2X2,3-4X4,4-1X2,5-,6-1X4\n"),
		makeFormatQString("SampleTime = %.3f\t\t\t; 采样积分时间（秒）\n", float(d_panel->getSampleTime()) / 1000),
		makeFormatQString("FrameCnt=%d\t\t\t;单分度采样帧数\n", d_framesPerGraduation),
		QString::fromLocal8Bit("DataType=INT16\t\t\t;采集数据类型，INT16，INT32，FLOAT\n\n"),

		QString::fromLocal8Bit(";--------------------------------------------;\n"),
		QString::fromLocal8Bit(";  扫描几何参数\n"),
		QString::fromLocal8Bit(";  以转轴为z轴，垂直z轴的射线为y轴（方向指向射线源），建立全局坐标系；\n"),
		QString::fromLocal8Bit(";  探测器水平方向为u轴，垂直方向为v轴，建立局部坐标系；\n"),
		QString::fromLocal8Bit(";  局部坐标系绕全局系z轴旋转。\n"),
		QString::fromLocal8Bit(";--------------------------------------------;\n"),
		QString::fromLocal8Bit("[GEOMETRY]\n"),
		makeFormatQString("BlockPixelLeft=%d\t\t\t;面阵拼接扫描左侧屏蔽的像素数\n", d_pandetData.pixelBlockLeft),
		makeFormatQString("BlockPixelRight=%d\t\t\t;面阵拼接扫描右侧屏蔽的像素数\n", d_pandetData.pixelBlockRight),

		makeFormatQString("SOD=%.1f\t\t\t;射线源焦点到旋转轴的实际距离（mm）\n", d_controller->readAxisPostion(Axis::objRadial)),
		makeFormatQString("SDD=%.1f\t\t\t;射线源焦点到探测器的实际距离（mm）\n", d_controller->readAxisPostion(Axis::detRadial)),
		makeFormatQString("thetaY=%.2f\t\t\t;探测器绕y轴的旋转角度（度）\n", 0.0f),
		makeFormatQString("thetaX=%.2f\t\t\t;探测器绕x轴的旋转角度（度）\n", 0.0f),
		makeFormatQString("thetaZ=%.2f\t\t\t;探测器绕z轴的旋转角度（度）\n", 0.0f),
		makeFormatQString("uo_mm=%.1f\t\t\t;中心探测器水平坐标（mm）\n", d_pandetData.pixelSize * d_width / 2),
		makeFormatQString("vo_mm=%.1f\t\t\t;中心探测器垂直坐标（mm）\n", d_pandetData.pixelSize * d_height / 2),
		makeFormatQString("graduation=%d\t\t\t;分度数\n", d_graduation),
		makeFormatQString("Angle00000=%d\t\t\t;0号投影的角度\n", d_orientInc),
		makeFormatQString("…\t\t\t\t; 1-%04u号投影的角度\n", d_graduation),
		makeFormatQString("Angle%04u=%d\t\t\t;%04u号投影的角度\n\n",	d_graduation, d_orientInc + 360, d_graduation),

		QString::fromLocal8Bit(";---------------------\n"),
		QString::fromLocal8Bit(";  重建图像参数\n"),
		QString::fromLocal8Bit(";---------------------\n"),
		QString::fromLocal8Bit("[IMAGE PARAMETER]\n"),
		makeFormatQString("Width=%d\t\t\t;重建图像的宽度或直径（像素）\n", 800),
		makeFormatQString("StartLayer=%d\t\t\t;重建图像的起始层（像素）\n", 0),
		makeFormatQString("Height=%d\t\t\t;重建图像的高度或层数（像素）\n", 800),
		makeFormatQString("SkipGraduation=%d\t\t\t;分度跳跃数\n", 1),
		makeFormatQString("SkipHorizontal=%d\t\t\t;探测器水平方向跳跃数\n", 0),
		makeFormatQString("SkipVertical =%d\t\t\t;探测器垂直方向跳跃数\n", 0),
		makeFormatQString("rotAngle=%d\t\t\t;重建起始角度（°）\n", 0),
		makeFormatQString("FirstSection=%d\t\t\t;\n", 100),
		makeFormatQString("LastSection=%d\t\t\t;\n", 800),
		makeFormatQString("Filename=\n"),
		makeFormatQString("Image Directory=%s\t\t\t;\n", "C:\\"),
		makeFormatQString("Sigma=%.6f\t\t\t;\n", 2.0)
	};

	return true;
}

void ConeScanInterface::setFileName(const QString& _orgName, const QString& _destPath)
{
	d_orgName = _orgName;
	d_tunedFilePath = _destPath;
}

void ConeScanInterface::setTuneFlag(bool _bkgFlag, bool _airFlag, bool _defectFlag, bool _averageFlag)
{
	d_bkgTuneFlag = _bkgFlag;
	d_airTuneFlag = _airFlag;
	d_defectTuneFlag = _defectFlag;
	d_averageFlag = _averageFlag;
}

bool ConeScanInterface::stopScan()
{
	if (d_scanThread)
		d_scanThread->stopThread();

	if (d_imageProcessThread)
		d_imageProcessThread->stopThread();

	return true;
}

bool ConeScanInterface::beginScan(int _graduation, int _framesPerGraduation, int _round, int _posTime, int _cycleTime,
	unsigned short _gainFactor, float _orientInc, float _slicePos, float _sod, float _sdd)
{
	//if (!canScan())
	//	return false;

	//if(!writeParameterFile())
	//	return false;
	//
	//if (!loadBkgData())
	//	return false;

	//if (!loadAirData())
	//	return false;
	setCommonScanParameter(_graduation, _framesPerGraduation, _round, _posTime, _cycleTime, _gainFactor, _orientInc, _slicePos, _sod, _sdd);
	sendCmdToController();
	detachScanProcessThread();
	return true;
}

void ConeScanInterface::getScanProgress(int & _thisRound, int & _allProgress, QString & imagesCollectedAndSpaceOccupied)
{
	_thisRound = 100 * d_graduationCount / d_graduation;
	_allProgress = 100 * d_graduationCount / d_graduation;
	imagesCollectedAndSpaceOccupied = makeFormatQString("已经采集%d幅，占用硬盘空间%dm", int(d_graduationCount),
		size_t(d_graduation) * d_frameSize * d_framesPerGraduation / (1024 * 1024));
}
