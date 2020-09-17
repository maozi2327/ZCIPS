#include "stdafx.h"
#include "PanelImageProcess.h"
#include "../Public/headers/MicroDataCorrection.h"
#include "../Public/headers/tiffio.h"
#include <algorithm>
#include <QFileInfo>

class PanelImageProcess::Impl
{
public:
	unsigned short* d_bkgData;
	unsigned short* d_airData;
	unsigned short* d_defectData;
	unsigned short* d_correctedData;
	float* d_correctedDataFloat;
	void* d_correctedDataVoid;
	QString d_bkg;
};

PanelImageProcess::PanelImageProcess()
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

int PanelImageProcess::bkgCorrectFileToFile(const QString& _sourceFile, const QString& _destFile)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	QByteArray destFileName = _destFile.toLocal8Bit();
	return _nCTBKGCorrection(nullptr, sourceFileName.data(), d_impl->d_bkgData, nullptr, nullptr, destFileName.data(), 0, 0);
}

int PanelImageProcess::bkgCorrectFileToData(const QString& _sourceFile)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	return _nCTBKGCorrection(nullptr, sourceFileName.data(), d_impl->d_bkgData, nullptr, d_impl->d_correctedData, nullptr, 0, 0);
}

int PanelImageProcess::bkgCorrectDataToFile(unsigned short * _sourceData, const QString& _destFile, int _line, int _column)
{
	QByteArray destFileName = _destFile.toLocal8Bit();
	return _nCTBKGCorrection(_sourceData, nullptr, d_impl->d_bkgData, nullptr, nullptr, destFileName.data(), _line, _column);
}

int PanelImageProcess::bkgCorrectDataToData(unsigned short * _sourceData, int _line, int _column)
{
	return _nCTBKGCorrection(_sourceData, nullptr, d_impl->d_bkgData, nullptr, d_impl->d_correctedData, nullptr, _line, _column);
}

int PanelImageProcess::airCorrectFileToFile(const QString& _sourceFile, const QString& _destFile)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	QByteArray destFileName = _destFile.toLocal8Bit();
	return _nCTAIRCorrection(nullptr, sourceFileName.data(), d_impl->d_airData, nullptr, nullptr, destFileName.data(), 0, 0);
}

int PanelImageProcess::airCorrectFileToData(const QString& _sourceFile)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	return _nCTAIRCorrection(nullptr, sourceFileName.data(), d_impl->d_airData, nullptr, d_impl->d_correctedDataFloat, nullptr, 0, 0);
}

int PanelImageProcess::airCorrectDataToFile(unsigned short * _sourceData, const QString& _destFile, int _line, int _column)
{
	QByteArray destFileName = _destFile.toLocal8Bit();
	return _nCTAIRCorrection(_sourceData, nullptr, d_impl->d_airData, nullptr, nullptr, destFileName.data(), _line, _column);
}

int PanelImageProcess::airCorrectDataToData(unsigned short * _sourceData, int _line, int _column)
{
	return _nCTAIRCorrection(_sourceData, nullptr, d_impl->d_bkgData, nullptr, d_impl->d_correctedDataFloat, nullptr, _line, _column);
}

int PanelImageProcess::defectCorrectFileToFile(const QString& _sourceFile, const QString& _destFile)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	QByteArray destFileName = _destFile.toLocal8Bit();
	return _nCTDefectCorrection(nullptr, sourceFileName.data(), d_impl->d_defectData, nullptr, nullptr, destFileName.data(), 0, 0);
}

int PanelImageProcess::defectCorrectFileToData(const QString& _sourceFile)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	return _nCTDefectCorrection(nullptr, sourceFileName.data(), d_impl->d_defectData, nullptr, d_impl->d_correctedDataVoid, nullptr, 0, 0);
}

int PanelImageProcess::defectCorrectDataToFile(unsigned short * _sourceData, const QString& _destFile, int _line, int _column)
{
	QByteArray destFileName = _destFile.toLocal8Bit();
	return _nCTDefectCorrection(_sourceData, nullptr, d_impl->d_defectData, nullptr, nullptr, destFileName.data(), _line, _column);
}

int PanelImageProcess::defectCorrectDataToData(unsigned short * _sourceData, int _line, int _column)
{
	return _nCTDefectCorrection(_sourceData, nullptr, d_impl->d_defectData, nullptr, d_impl->d_correctedDataVoid, nullptr, _line, _column);
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
	return false;
}

int PanelImageProcess::dataSplice(const QString & _pathA, const QString & _pathB, const QString& _pathFinal, int& progress)
{
	QByteArray pathA = _pathA.toLocal8Bit();
	QByteArray pathB = _pathB.toLocal8Bit();
	QByteArray pathFinal = _pathFinal.toLocal8Bit();
	return 0;//_nCTDataSplicing_Detector(pathA.data(), pathB.data(), pathFinal.data(), &progress);
}

bool PanelImageProcess::loadBkgData(const QString& d_bkgFileName)
{

	return false;
}

bool PanelImageProcess::loadAirData(const QString& d_airFileName)
{
	if (d_impl->d_airData)
		delete[] d_impl->d_airData;

	loadTiff(d_airFileName, d_impl->d_airData);
	return false;
}
bool PanelImageProcess::loadDefectData(const QString& d_bkgFileName)
{

	return false;
}

std::unique_ptr<PanelImageProcess> PanelImageProcessFactory::getPanelImageProcess()
{
	return std::unique_ptr<PanelImageProcess>(new PanelImageProcess);
}