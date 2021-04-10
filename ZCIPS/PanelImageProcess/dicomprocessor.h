#pragma once
#pragma comment(lib, "dcmtk.lib")
#include <chrono>
#include <memory>
#include <list>
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmdata/dctagkey.h>
#include <dcmtk/dcmdata/dcfilefo.h>

struct Patient
{
	std::string                             PatientsName;
	std::string                             PatientID;
	std::chrono::system_clock::time_point   PatientsBirthDate;
	std::string                             PatientsSex;
	std::string                             OtherPatientNames;
	std::string                             PatientsWeight;
	std::string                             PatientComments;
};

struct Study
{
	std::string                             StudyInstanceUID;
	std::string                             StudyID;
	std::chrono::system_clock::time_point   StudyDateTime;
	std::string                             AccessionNumber;
	std::string                             StudyDescription;
};

struct Series
{
	std::string                             SeriesInstanceUID;
	std::string                             SeriesNumber;
	std::chrono::system_clock::time_point   SeriesDateTime;
	std::string                             PerformingPhysicianName;
	std::string                             ProtocolName;
	std::string                             BodyPartExamined;
};

struct Image
{

	unsigned int                            SamplesPerPixel;
	std::string                             PhotometricInterpretation;
	unsigned int                            Rows;
	unsigned int                            Columns;
	unsigned int                            BitsAllocated;
	unsigned int                            BitsStored;
	unsigned int                            HighBit;
	unsigned int                            PixelRepresentation;
	unsigned short*                         PixelData;
};

struct PrivateData
{
	uint16_t                                GroupTag;
	uint16_t                                ElementTag;
	std::string                             Name;
	std::string                             VR;
	std::string                             Value;
};

#define PRIVATE_GROUP_NAME "Private Group"

enum UID_Type
{
	UID_Study = 1,
	UID_Series = 2,
	UID_Image = 3,
	UID_Other = 4,
};

enum DataVR_Type
{
	DataVRType_CS = 1,
	DataVRType_SH,
	DataVRType_LO,
	DataVRType_ST,
	DataVRType_LT,
	DataVRType_UT,
	DataVRType_AE,
	DataVRType_PN,
	DataVRType_UI,
	DataVRType_DA,
	DataVRType_TM,
	DataVRType_DT,
	DataVRType_AS,
	DataVRType_IS,
	DataVRType_DS,
	DataVRType_SS,
	DataVRType_US,
	DataVRType_SL,
	DataVRType_UL,
	DataVRType_AT,
	DataVRType_FL,
	DataVRType_FD,
	DataVRType_OB,
	DataVRType_OW,
	DataVRType_OF,
	DataVRType_SQ,
	DataVRType_UN
};

std::string GenerateUniqueId(UID_Type type);
std::string GetDataVR(DataVR_Type type);
std::chrono::system_clock::time_point StringtoDate(std::string date);
std::chrono::system_clock::time_point StringtoDateTime(std::string date, std::string time);
std::string DatetoString(std::chrono::system_clock::time_point timePoint);
std::string TimetoString(std::chrono::system_clock::time_point timePoint);

class  DicomProcessor
{
public:
	DicomProcessor();
	~DicomProcessor();

	bool Read(std::string filePath);
	bool Save(std::string filePath);
	void Reset();

	inline std::shared_ptr<Patient> PatientInfo()
	{
		return m_patient;
	}

	inline std::shared_ptr<Study> StudyInfo()
	{
		return m_study;
	}

	inline std::shared_ptr<Series> SeriesInfo()
	{
		return m_series;
	}

	inline std::shared_ptr<Image> ImageInfo()
	{
		return m_image;
	}

	inline std::list<PrivateData> &PrivateDataInfo()
	{
		return m_privateData;
	}

	void MetaInfoTags();
	void DataSetTags();
	void ReadDicom(std::string file);
	void SaveDicom(std::string file);
	
private:
	void GetPatient();
	void GetStudy();
	void GetSeries();
	void GetImage();
	void GetPrivateData();

	void FillPatient();
	void FillStudy();
	void FillSeries();
	void FillImage();
	void FillPrivateData();

	void GetInfo(DcmTagKey tagKey, std::string &info);
	void GetInfo(DcmTagKey tagKey, unsigned int &info);
	void GetDateTimeInfo(DcmTagKey tagKey, DcmTagKey tagTimeKey, std::chrono::system_clock::time_point &info);
	void FillInfo(DcmTagKey tagKey, std::string &info);
	void RegisterPrivateTags(PrivateData data);
	void FreePixelData();

	std::string                 m_fileName;
	DcmDataset					m_dataSet;
	DcmFileFormat               m_fileformat;
	std::shared_ptr<Patient>    m_patient;
	std::shared_ptr<Study>      m_study;
	std::shared_ptr<Series>     m_series;
	std::shared_ptr<Image>      m_image;
	std::list<PrivateData>      m_privateData;
};