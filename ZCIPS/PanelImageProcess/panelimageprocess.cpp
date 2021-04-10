#include "stdafx.h"
#include "PanelImageProcess.h"
#include "../Public/headers/MicroDataCorrection.h"
#include "../Public/headers/tiffio.h"
#include <algorithm>
#include <QFileInfo>
#include "../Public/headers/FreeImage.h"
#include "../Public/util/functions.h"
#include "../Public/util/messagebox.h"
#include "dicomprocessor.h"
#pragma comment(lib, "./public/lib/MicroDataCorrectionDll.lib")
#pragma comment(lib, "./public/lib/tiff.lib")
#pragma comment(lib, "./public/lib/FreeImage.lib")

class PanelImageProcess::Impl
{
public:
	QString d_orgPath;
	QString d_ctPath;
	QString d_drPath;
	QString d_airPath;
	QString d_bkgPath;
	QString d_defectPath;
	QString d_bkg;

	unsigned short* d_bkgData;
	float* d_airData;
	unsigned short* d_defectData;
	unsigned short* d_correctedData;
	float* d_correctedDataFloat;
	void* d_correctedDataVoid;
	int d_width;
	int d_height;
	std::unique_ptr<DicomProcessor> d_dicomProcessor;

	Impl(int _width, int _height)
		: d_dicomProcessor(new DicomProcessor())
	{
		d_width = _width;
		d_height = _height;
		d_bkgData = nullptr;
		d_airData = nullptr;
		d_defectData = nullptr;
		d_correctedData = new unsigned short[d_width * d_height];
		//d_dicomProcessor->ReadDicom("D:\\image-000001.dcm");
		//d_dicomProcessor->SaveDicom("D:\\image-000002.dcm");
	}

	~Impl()
	{
		if (!d_bkgData)
			free(d_bkgData);

		if (!d_airData)
			free(d_airData);

		if (!d_defectData)
			free(d_defectData);

		if (!d_correctedData)
			delete[] d_correctedData;
	}
};

PanelImageProcess::PanelImageProcess(int _width, int _height) : d_impl(new Impl(_width, _height))
{
	
}


PanelImageProcess::~PanelImageProcess()
{
}

bool PanelImageProcess::loadTiff(const QString & _sourceFile, unsigned short*& _data)
{
	QFileInfo info(_sourceFile);
	size_t fileSize = 0;

	if (info.exists())
		fileSize = info.size();
	else
		return false;

	QByteArray fileName = _sourceFile.toLocal8Bit();
	TIFF* tif = TIFFOpen(fileName.data(), "r");

	if (!tif)
		return false;

	int dirCount = 0;
	char* buf = new char[fileSize];
	
	size_t bytesRead = 0;

	do
	{
		++dirCount;
		unsigned long long* bc;
		TIFFGetField(tif, TIFFTAG_STRIPBYTECOUNTS, &bc);
		uint32 stripSize;
		stripSize = bc[0];
		size_t stripCount = TIFFNumberOfStrips(tif);

		for (tstrip_t strip = 0; strip < stripCount; strip++)
		{
			TIFFReadRawStrip(tif, strip, buf + bytesRead, bc[strip]);
			bytesRead += bc[strip];
		}
	}
	while (TIFFReadDirectory(tif));

	realloc(buf, bytesRead);
	TIFFClose(tif);
	_data = (unsigned short*)(buf);
	return true;
}

bool PanelImageProcess::saveTiff(const QString & _sourceFile)
{
	return false;
}


int PanelImageProcess::createBkgFileToFile(const QString& _sourceFile, const QString& _destFile)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	QByteArray destFileName = _destFile.toLocal8Bit();
	return _nCTCreateBKGData(nullptr, sourceFileName.data(), nullptr, destFileName.data(), 0, 0, 0);
}

int PanelImageProcess::createBkgFileToData(const QString& _sourceFile)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	return _nCTCreateBKGData(nullptr, sourceFileName.data(), d_impl->d_correctedData, nullptr, 0, 0, 0);
}

int PanelImageProcess::createBkgDataToFile(unsigned short** _sourceData, const QString& _destFile, int _line,
	int _column, int _frames)
{
	QByteArray destFileName = _destFile.toLocal8Bit();
	return _nCTCreateBKGData(_sourceData, nullptr, nullptr, destFileName.data(), _line, _column, _frames);
}

int PanelImageProcess::createBkgDataToData(unsigned short** _sourceData, unsigned short * _destData,
	int _line, int _column, int _frames)
{
	return _nCTCreateBKGData(_sourceData, nullptr, _destData, nullptr, _line, _column, _frames);
}

int PanelImageProcess::createAirFileToFile(const QString& _sourceFile, const QString& _destFile, const QString& _bkgFile)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	QByteArray destFileName = _destFile.toLocal8Bit();
	QByteArray bkgFileName = _bkgFile.toLocal8Bit();
	return _nCTCreateAIRData(nullptr, sourceFileName.data(), nullptr, destFileName.data(),
		nullptr, bkgFileName.data(), 0, 0, 0);
}

int PanelImageProcess::createAirFileToData(const QString& _sourceFile, const QString& _bkgFile)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	QByteArray bkgFileName = _bkgFile.toLocal8Bit();
	return _nCTCreateAIRData(nullptr, sourceFileName.data(), d_impl->d_correctedDataFloat, nullptr,
		nullptr, bkgFileName.data(), 0, 0, 0);
}

int PanelImageProcess::createAirDataToFile(unsigned short** _sourceData, const QString& _destFile, const QString& _bkgFile, int _line,
	int _column, int _frames)
{
	QByteArray destFileName = _destFile.toLocal8Bit();
	QByteArray bkgFileName = _bkgFile.toLocal8Bit();
	return _nCTCreateAIRData(_sourceData, nullptr, nullptr, destFileName.data(),
		nullptr, bkgFileName.data(), _line, _column, _frames);
}

int PanelImageProcess::createAirDataToData(unsigned short** _sourceData, const QString& _bkgFile,
	int _line, int _column, int _frames)
{
	QByteArray bkgFileName = _bkgFile.toLocal8Bit();
	return _nCTCreateAIRData(_sourceData, nullptr, d_impl->d_correctedDataFloat, nullptr,
		nullptr, bkgFileName.data(), _line, _column, _frames);
}

int PanelImageProcess::bkgTuneFileToFile(const QString& _sourceFile, const QString& _destFile)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	QByteArray destFileName = _destFile.toLocal8Bit();
	return _nCTBKGCorrection(nullptr, sourceFileName.data(), d_impl->d_bkgData, nullptr, nullptr, destFileName.data(), 0, 0);
}

int PanelImageProcess::bkgTuneFileToData(const QString& _sourceFile)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	return _nCTBKGCorrection(nullptr, sourceFileName.data(), d_impl->d_bkgData, nullptr, d_impl->d_correctedData, nullptr, 0, 0);
}

int PanelImageProcess::bkgTuneDataToFile(unsigned short * _sourceData, const QString& _destFile, int _line, int _column)
{
	QByteArray destFileName = _destFile.toLocal8Bit();
	return _nCTBKGCorrection(_sourceData, nullptr, d_impl->d_bkgData, nullptr, nullptr, destFileName.data(), _line, _column);
}

int PanelImageProcess::bkgTuneDataToData(unsigned short * _sourceData, int _line, int _column)
{
	return _nCTBKGCorrection(_sourceData, nullptr, d_impl->d_bkgData, nullptr, d_impl->d_correctedData, nullptr, _line, _column);
}

int PanelImageProcess::airTuneFileToFile(const QString& _sourceFile, const QString& _destFile)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	QByteArray destFileName = _destFile.toLocal8Bit();
	return _nCTAIRCorrection(nullptr, sourceFileName.data(), nullptr, destFileName.data(), d_impl->d_airData, 0, 0, 0);
}

int PanelImageProcess::airTuneFileToData(const QString& _sourceFile)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	return _nCTAIRCorrection(nullptr, sourceFileName.data(), (unsigned short*)d_impl->d_correctedDataVoid, nullptr, d_impl->d_airData, nullptr, 0, 0);
}

int PanelImageProcess::airTuneDataToFile(unsigned short * _sourceData, const QString& _destFile, int _line, int _column)
{
	QByteArray destFileName = _destFile.toLocal8Bit();
	return _nCTAIRCorrection(_sourceData, nullptr, nullptr, destFileName.data(), d_impl->d_airData, nullptr, _line, _column);
}

int PanelImageProcess::airTuneDataToData(unsigned short * _sourceData, int _line, int _column)
{
	return _nCTAIRCorrection(_sourceData, nullptr, d_impl->d_bkgData, nullptr, d_impl->d_correctedDataFloat, nullptr, _line, _column);
}

int PanelImageProcess::defectTuneFileToFile(const QString& _sourceFile, const QString& _destFile)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	QByteArray destFileName = _destFile.toLocal8Bit();
	return _nCTDefectCorrection(nullptr, sourceFileName.data(), d_impl->d_defectData, nullptr, nullptr, destFileName.data(), 0, 0);
}

int PanelImageProcess::defectTuneFileToData(const QString& _sourceFile)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	return _nCTDefectCorrection(nullptr, sourceFileName.data(), d_impl->d_defectData, nullptr, d_impl->d_correctedDataVoid, nullptr, 0, 0);
}

int PanelImageProcess::defectTuneDataToFile(unsigned short * _sourceData, const QString& _destFile, int _line, int _column)
{
	QByteArray destFileName = _destFile.toLocal8Bit();
	return _nCTDefectCorrection(_sourceData, nullptr, d_impl->d_defectData, nullptr, nullptr, destFileName.data(), _line, _column);
}

int PanelImageProcess::defectTuneDataToData(unsigned short * _sourceData, int _line, int _column)
{
	return _nCTDefectCorrection(_sourceData, nullptr, d_impl->d_defectData, nullptr, d_impl->d_correctedDataVoid, nullptr, _line, _column);
}

int PanelImageProcess::bkgAirTuneDataToData(unsigned short * _sourceData, int _line, int _column)
{
	int ret = _nCTBKGCorrection(_sourceData, nullptr, d_impl->d_correctedData, nullptr, d_impl->d_bkgData, nullptr, _line, _column);

	if (ret != 0)
		return ret;

	return _nCTAIRCorrection(d_impl->d_correctedData, nullptr, d_impl->d_correctedData, nullptr, d_impl->d_airData, nullptr, _line, _column);
}

int PanelImageProcess::bkgAirTuneDataToFile(unsigned short * _sourceData, const QString & _destFile, int _line, int _column)
{
	int ret = _nCTBKGCorrection(_sourceData, nullptr, d_impl->d_correctedData, nullptr, d_impl->d_bkgData, nullptr, _line, _column);

	if (ret != 0)
		return ret;

	QByteArray fileName = replaceSlashToBslash(_destFile).toLocal8Bit();
	return _nCTAIRCorrection(d_impl->d_correctedData, nullptr, nullptr, fileName.data(), d_impl->d_airData, nullptr, _line, _column);
}

int PanelImageProcess::bkgAirDefectTuneDataToData(unsigned short * _sourceData, int _line, int _column)
{
	int ret = _nCTBKGCorrection(_sourceData, nullptr, d_impl->d_correctedData, nullptr, d_impl->d_bkgData, nullptr, _line, _column);

	if (ret != 0)
		return ret;

	ret = _nCTAIRCorrection(d_impl->d_correctedData, nullptr, d_impl->d_correctedData, nullptr, d_impl->d_airData, nullptr, _line, _column);

	if (ret != 0)
		return ret;

	return _nCTDefectCorrection(d_impl->d_correctedData, nullptr, d_impl->d_correctedData, nullptr, d_impl->d_defectData, nullptr, _line, _column);
}

int PanelImageProcess::bkgAirDefectTuneDataToFile(unsigned short* _sourceData, const QString& _destFile, int _line, int _column)
{
	int ret = _nCTBKGCorrection(_sourceData, nullptr, d_impl->d_correctedData, nullptr, d_impl->d_bkgData, nullptr, _line, _column);

	if (ret != 0)
		return ret;

	ret = _nCTAIRCorrection(d_impl->d_correctedData, nullptr, d_impl->d_correctedData, nullptr, d_impl->d_airData, nullptr, _line, _column);

	if (ret != 0)
		return ret;

	QByteArray fileName = replaceSlashToBslash(_destFile).toLocal8Bit();
	return _nCTDefectCorrection(d_impl->d_correctedData, nullptr, nullptr, fileName.data(), d_impl->d_defectData, nullptr, _line, _column);
}

int PanelImageProcess::binProcessFileToFile(const QString& _sourceFile, const QString& _destFile, int _binWidth, int _binHeight)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	QByteArray destFileName = _destFile.toLocal8Bit();
	return _nCTBinModeProcess(nullptr, sourceFileName.data(), nullptr, destFileName.data(), 0, 0, _binWidth, _binHeight);
}

int PanelImageProcess::binProcessFileToData(const QString& _sourceFile, int _binWidth, int _binHeight)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	return _nCTBinModeProcess(nullptr, sourceFileName.data(), d_impl->d_correctedData, nullptr, 0, 0, _binWidth, _binHeight);
}

int PanelImageProcess::binProcessDataToFile(unsigned short * _sourceData, const QString& _destFile,
	int _line, int _column, int _binWidth, int _binHeight)
{
	QByteArray destFileName = _destFile.toLocal8Bit();
	return _nCTBinModeProcess(_sourceData, nullptr, nullptr, destFileName.data(), _line, _column,
		_binHeight, _binHeight);
}

int PanelImageProcess::binProcessDataToData(unsigned short * _sourceData, int _line, int _column
	, int _binWidth, int _binHeight)
{
	return _nCTBinModeProcess(_sourceData, nullptr, d_impl->d_correctedData, nullptr,
		_line, _column, _binWidth, _binHeight);
}

int PanelImageProcess::rotateProcessFileToFile(const QString& _sourceFile, const QString& _destFile
	, int _iRotationType, int _iRotationAngle)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	QByteArray destFileName = _destFile.toLocal8Bit();
	return _nCTRotationProcess(nullptr, sourceFileName.data(), nullptr, destFileName.data(),
		_iRotationType, _iRotationAngle, 0, 0);
}

int PanelImageProcess::rotateProcessFileToData(const QString& _sourceFile, int _iRotationType, int _iRotationAngle)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	return _nCTRotationProcess(nullptr, sourceFileName.data(), d_impl->d_correctedData, nullptr,
		_iRotationType, _iRotationAngle, 0, 0);
}

int PanelImageProcess::rotateProcessDataToFile(unsigned short * _sourceData, const QString& _destFile,
	int _line, int _column, int _iRotationType, int _iRotationAngle)
{
	QByteArray destFileName = _destFile.toLocal8Bit();
	return _nCTRotationProcess(_sourceData, nullptr, nullptr, destFileName.data(),
		_iRotationType, _iRotationAngle, _line, _column);
}

int PanelImageProcess::rotateProcessDataToData(unsigned short * _sourceData,
	int _line, int _column, int _iRotationType, int _iRotationAngle)
{
	return _nCTRotationProcess(_sourceData, nullptr, d_impl->d_correctedData, nullptr,
		_iRotationType, _iRotationAngle, _line, _column);
}

int PanelImageProcess::mirrorYDataToFile(unsigned short * _sourceData, const QString & _destFile, int _line, int _column)
{
	return 0;
}

int PanelImageProcess::mirrorYDataToData(unsigned short * _sourceData, int _line, int _column)
{
	for (int i = 0; i !=  _line; ++i)
		for (int j = 0; j != _column; ++j)
			*(d_impl->d_correctedData + i * _column + j) = *(_sourceData + i * _column + _column - j - 1);

	memcpy(_sourceData, d_impl->d_correctedData, _line * _column * 2);
	return 0;
}

int PanelImageProcess::mergeProcessFileToFile(const QString& _sourceFile, const QString& _destFile)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	QByteArray destFileName = _destFile.toLocal8Bit();
	return _nCTMergeFrames(nullptr, sourceFileName.data(), nullptr, destFileName.data(), 0, 0, 0);
}

int PanelImageProcess::mergeProcessFileToData(const QString& _sourceFile)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	return _nCTMergeFrames(nullptr, sourceFileName.data(), d_impl->d_correctedData, nullptr, 0, 0, 0);
}

int PanelImageProcess::mergeProcessDataToFile(unsigned short** _sourceData, const QString& _destFile,
	int _frames, int _line, int _column)
{
	QByteArray destFileName = _destFile.toLocal8Bit();
	return _nCTMergeFrames(_sourceData, nullptr, nullptr, destFileName.data(), _frames, _line, _column);
}

int PanelImageProcess::mergeProcessDataToData(unsigned short** _sourceData, int _frames, int _line, int _column)
{
	return _nCTMergeFrames(_sourceData, nullptr, d_impl->d_correctedData, nullptr, _frames, _line, _column);
}

bool PanelImageProcess::saveMultiBitmapDataToFile(unsigned short * _sourceData, const QString& _destFile, int _frames, int _line, int _column)
{
	return false;
}

bool PanelImageProcess::saveSingleBitmapDataToFile(unsigned short * _sourceData, const QString& _destFile, int _line, int _column)
{
	FIBITMAP *FIBmpSingle = NULL;
	unsigned short *pusTIFFData;
	FREE_IMAGE_TYPE FIType = FIT_UINT16;
	FREE_IMAGE_FORMAT FIFormat = FIF_TIFF;
	FIBmpSingle = FreeImage_AllocateT(FIType, _line, _column, 1);
	pusTIFFData = (unsigned short *)FreeImage_GetBits(FIBmpSingle);
	memcpy((void *)pusTIFFData, (const void *)_sourceData, _line * _column * sizeof(unsigned short));
	QByteArray byteArray = _destFile.toLocal8Bit();
	FreeImage_Save(FIFormat, FIBmpSingle, byteArray.data(), TIFF_NONE);
	FreeImage_Unload(FIBmpSingle);
	return true;
}

int PanelImageProcess::dataSplice(const QString & _pathA, const QString & _pathB, const QString& _pathFinal, int& progress)
{
	QByteArray pathA = _pathA.toLocal8Bit();
	QByteArray pathB = _pathB.toLocal8Bit();
	QByteArray pathFinal = _pathFinal.toLocal8Bit();
	return _nCTDataSplicing_Detector(pathA.data(), pathB.data(), pathFinal.data(), &progress);
}

bool PanelImageProcess::loadBkgData(const QString& d_bkgFileName)
{
	QByteArray byteArray = replaceSlashToBslash(d_bkgFileName).toLocal8Bit();
	FIBITMAP* FIBmpGet = NULL;
	FREE_IMAGE_FORMAT FIFormat = FIF_TIFF;

	if (FIBmpGet = FreeImage_Load(FIFormat, byteArray.data())) 
	{
		unsigned short* pusTIFFData = (unsigned short *)FreeImage_GetBits(FIBmpGet);
		unsigned long size = FreeImage_GetHeight(FIBmpGet) * FreeImage_GetWidth(FIBmpGet) * sizeof(unsigned short);

		if(d_impl->d_bkgData == nullptr)
			d_impl->d_bkgData = (unsigned short *)malloc(size);

		memcpy(d_impl->d_bkgData, pusTIFFData, size);
		FreeImage_Unload(FIBmpGet);
		return true;
	}

	return false;
	//TODO_DJ:libTIff
	//return loadTiff(d_impl->d_bkgPath + d_bkgFileName, d_impl->d_bkgData);
	//TODO_DJ:libTIff
}

bool PanelImageProcess::loadAirData(const QString& d_airFileName)
{
	QByteArray byteArray = replaceSlashToBslash(d_airFileName).toLocal8Bit();
	FIBITMAP* FIBmpGet = NULL;
	FREE_IMAGE_FORMAT FIFormat = FIF_TIFF;

	if (FIBmpGet = FreeImage_Load(FIFormat, byteArray.data()))
	{
		float* pusAirData = (float *)FreeImage_GetBits(FIBmpGet);
		unsigned long size = FreeImage_GetHeight(FIBmpGet) * FreeImage_GetWidth(FIBmpGet) * sizeof(float);

		if(d_impl->d_airData == nullptr)
			d_impl->d_airData = (float*)malloc(size);

		memcpy(d_impl->d_airData, pusAirData, size);
		FreeImage_Unload(FIBmpGet);
		return true;
	}

	return false;
	//TODO_DJ:libTIff
	//return loadTiff(d_impl->d_airPath + d_airFileName, d_impl->d_airData);
	//TODO_DJ:libTIff
}

bool PanelImageProcess::loadDefectData(const QString& d_defectFileName)
{
	QFile defectFile(d_defectFileName);
	
	if (!defectFile.exists())
	{
		messageBox(QString::fromLocal8Bit("坏像素文件不存在！"), QString::fromLocal8Bit("!!!!"));
		return false;
	}
	
	defectFile.open(QIODevice::ReadOnly);
	QByteArray defectData = defectFile.readAll();
	auto size = defectData.size();

	if (d_impl->d_defectData == nullptr)
	{
		d_impl->d_defectData = (unsigned short*)malloc(defectData.size());
	}

	memcpy(d_impl->d_defectData, defectData.data(), defectData.size());
	return true;
	//TODO_DJ:libTIff
	//return loadTiff(d_impl->d_defectPath + d_defectFileName, d_impl->d_defectData);
	//TODO_DJ:libTIff
}

bool PanelImageProcess::saveSingleBitmapDataToDicomFile(unsigned short * _sourceData, const QString & _destFile, int _line, int _column, 
	const QString& _objectName, const QString& _number)
{
	QByteArray byteName = _objectName.toLocal8Bit();
	std::string nameString = byteName.data();
	d_impl->d_dicomProcessor->PatientInfo()->PatientsName = nameString;
	byteName = _number.toLocal8Bit();
	nameString = byteName.data();
	d_impl->d_dicomProcessor->PatientInfo()->PatientID = std::string(nameString);
	d_impl->d_dicomProcessor->ImageInfo()->Columns = _column;
	d_impl->d_dicomProcessor->ImageInfo()->Rows = _line;
	d_impl->d_dicomProcessor->ImageInfo()->PixelData = _sourceData;
	byteName = replaceSlashToBslash(_destFile).toLocal8Bit();
	nameString = byteName.data();
	return d_impl->d_dicomProcessor->Save(nameString);
}

bool PanelImageProcess::bkgAirDefectTuneDataToDicomFile(unsigned short * _sourceData, const QString & _destFile, int _line, int _column, const QString& _objectName
	, const QString& _number)
{
	bkgAirDefectTuneDataToData(_sourceData, _line, _column);
	return saveSingleBitmapDataToDicomFile(d_impl->d_correctedData, _destFile, _line, _column, _objectName, _number);
}

//std::unique_ptr<PanelImageProcess> PanelImageProcessFactory::getPanelImageProcess(int _width, int _height)
//{
//	return std::unique_ptr<PanelImageProcess>(new PanelImageProcess(_width, _height));
//}