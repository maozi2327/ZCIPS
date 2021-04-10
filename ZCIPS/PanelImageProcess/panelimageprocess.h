#pragma once
#include "panelimageprocess_global.h"
#include <memory>

class PANELIMAGEPROCESS_EXPORT PanelImageProcess
{
private:
	class Impl;
	std::unique_ptr<Impl> d_impl;
	bool loadTiff(const QString& _sourceFile, unsigned short*& _data);
	bool saveTiff(const QString& _sourceFile);
public:
	PanelImageProcess(int _width, int _height);
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

	int bkgTuneFileToFile(const QString& _sourceFile, const QString& _destFile);
	int bkgTuneFileToData(const QString& _sourceFile);
	int bkgTuneDataToFile(unsigned short* _sourceData, const QString& _destFile, int _line, int _column);
	int bkgTuneDataToData(unsigned short* _sourceData, int _line, int _column);

	int airTuneFileToFile(const QString& _sourceFile, const QString& _destFile);
	int airTuneFileToData(const QString& _sourceFile);
	int airTuneDataToFile(unsigned short* _sourceData, const QString& _destFile, int _line, int _column);
	int airTuneDataToData(unsigned short* _sourceData, int _line, int _column);

	int defectTuneFileToFile(const QString& _sourceFile, const QString& _destFile);
	int defectTuneFileToData(const QString& _sourceFile);
	int defectTuneDataToFile(unsigned short* _sourceData, const QString& _destFile, int _line, int _column);
	int defectTuneDataToData(unsigned short* _sourceData, int _line, int _column);

	int bkgAirTuneDataToData(unsigned short* _sourceData, int _line, int _column);
	int bkgAirTuneDataToFile(unsigned short* _sourceData, const QString& _destFile, int _line, int _column);

	int bkgAirDefectTuneDataToData(unsigned short* _sourceData, int _line, int _column);
	int bkgAirDefectTuneDataToFile(unsigned short* _sourceData, const QString& _destFile, int _line, int _column);

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

	int mirrorYDataToFile(unsigned short* _sourceData, const QString& _destFile, int _line,	int _column);
	int mirrorYDataToData(unsigned short* _sourceData, int _line, int _column);

	int mergeProcessFileToFile(const QString& _sourceFile, const QString& _destFile);
	int mergeProcessFileToData(const QString& _sourceFile);
	int mergeProcessDataToFile(unsigned short** _sourceData, const QString& _destFile, int _frames, int _line, int _column);
	int mergeProcessDataToData(unsigned short** _sourceData, int _frames, int _line, int _column);

	bool saveMultiBitmapDataToFile(unsigned short* _sourceData, const QString& _destFile, int _frames, int _line, int _column);
	bool saveSingleBitmapDataToFile(unsigned short* _sourceData, const QString& _destFile, int _line, int _column);
	int dataSplice(const QString& _pathA, const QString& _pathB, const QString& _pathFinal, int& progress);

	bool loadBkgData(const QString& d_bkgFileName);
	bool loadAirData(const QString& d_airFileName);
	bool loadDefectData(const QString& d_defectFileName);

	bool bkgAirDefectTuneDataToDicomFile(unsigned short* _sourceData, const QString& _destFile, int _line, int _column, const QString& d_objectName, const QString& _number);
	bool saveSingleBitmapDataToDicomFile(unsigned short* _sourceData, const QString& _destFile, int _line, int _column, const QString& d_objectName, const QString& _number);
};
//
//class PANELIMAGEPROCESS_EXPORT PanelImageProcessFactory
//{
//public:
//	static std::unique_ptr<PanelImageProcess> getPanelImageProcess(int _width, int _height);
//};