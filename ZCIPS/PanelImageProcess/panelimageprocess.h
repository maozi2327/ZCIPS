#pragma once
#pragma comment(lib, "./public/lib/MicroDataCorrectionDll.lib")
#pragma comment(lib, "./public/lib/tiff.lib")

#include "panelimageprocess_global.h"
#include <QString>
class PANELIMAGEPROCESS_EXPORT PanelImageProcess
{
private:
	class Impl;
	std::unique_ptr<Impl> d_impl;
	bool loadTiff(const QString& _sourceFile, unsigned short*& _data);
	bool saveTiff(const QString& _sourceFile);
public:
	PanelImageProcess();
	~PanelImageProcess();

	int createBkgFileToFile(const QString& _sourceFile, const QString& _destFile);
	int createBkgFileToData(const QString& _sourceFile);
	int createBkgDataToFile(unsigned short** _sourceData, const QString& _destFile, int _line, int _column, int _frames);
	int createBkgDataToData(unsigned short** _sourceData, unsigned short* _destData,
		int _line, int _column, int _frames);

	int createAirFileToFile(const QString& _sourceFile, const QString& _destFile, const QString& d_bkgFile);
	int createAirFileToData(const QString& _sourceFile, const QString& d_bkgFile);
	int createAirDataToFile(unsigned short** _sourceData, const QString& _destFile, const QString& d_bkgFile, int _line,
		int _column, int _frames);
	int createAirDataToData(unsigned short** _sourceData, const QString& d_bkgFile,
		int _line, int _column, int _frames);

	int bkgCorrectFileToFile(const QString& _sourceFile, const QString& _destFile);
	int bkgCorrectFileToData(const QString& _sourceFile);
	int bkgCorrectDataToFile(unsigned short* _sourceData, const QString& _destFile, int _line, int _column);
	int bkgCorrectDataToData(unsigned short* _sourceData, int _line, int _column);

	int airCorrectFileToFile(const QString& _sourceFile, const QString& _destFile);
	int airCorrectFileToData(const QString& _sourceFile);
	int airCorrectDataToFile(unsigned short* _sourceData, const QString& _destFile, int _line, int _column);
	int airCorrectDataToData(unsigned short* _sourceData, int _line, int _column);

	int defectCorrectFileToFile(const QString& _sourceFile, const QString& _destFile);
	int defectCorrectFileToData(const QString& _sourceFile);
	int defectCorrectDataToFile(unsigned short* _sourceData, const QString& _destFile, int _line, int _column);
	int defectCorrectDataToData(unsigned short* _sourceData, int _line, int _column);

	int binProcessFileToFile(const QString& _sourceFile, const QString& _destFile, int _binWidth, int _binHeight);
	int binProcessFileToData(const QString& _sourceFile, int _binWidth, int _binHeight);
	int binProcessDataToFile(unsigned short* _sourceData, const QString& _destFile, int _line, int _column
		, int _binWidth, int _binHeight);
	int binProcessDataToData(unsigned short* _sourceData, int _line, int _column
		, int _binWidth, int _binHeight);

	int rotateProcessFileToFile(const QString& _sourceFile, const QString& _destFile, int _iRotationType, int iRotationAngle);
	int rotateProcessFileToData(const QString& _sourceFile, int _iRotationType, int iRotationAngle);
	int rotateProcessDataToFile(unsigned short* _sourceData, const QString& _destFile, int _line,
		int _column, int _iRotationType, int iRotationAngle);
	int rotateProcessDataToData(unsigned short* _sourceData, int _line, int _column,
		int _iRotationType, int iRotationAngle);

	int mergeProcessFileToFile(const QString& _sourceFile, const QString& _destFile);
	int mergeProcessFileToData(const QString& _sourceFile);
	int mergeProcessDataToFile(unsigned short** _sourceData, const QString& _destFile, int _frames, int _line, int _column);
	int mergeProcessDataToData(unsigned short** _sourceData, int _frames, int _line, int _column);

	bool saveMultiBitmapDataToFile(unsigned short* _sourceData, const QString& _destFile, int _frames, int _line, int _column);
	bool saveSingleBitmapDataToFile(unsigned short* _sourceData, const QString& _destFile, int _line, int _column);
	int dataSplice(const QString& _pathA, const QString& _pathB, const QString& _pathFinal, int& progress);
	bool loadBkgData(const QString& d_bkgFileName);
	bool loadAirData(const QString& d_bkgFileName);
	bool loadDefectData(const QString& d_bkgFileName);
};

class PANELIMAGEPROCESS_EXPORT PanelImageProcessFactory
{
public:
	static std::unique_ptr<PanelImageProcess> getPanelImageProcess();
};