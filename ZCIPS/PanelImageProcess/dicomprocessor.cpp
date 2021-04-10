#include "stdafx.h"
#include "dicomprocessor.h"
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcdict.h>
#include <dcmtk/dcmdata/dcdicent.h>
#include <dcmtk/dcmdata/dcmetinf.h>


std::string GenerateUniqueId(UID_Type type)
{
	char uid[100];
	switch (type)
	{
	case UID_Study:
		dcmGenerateUniqueIdentifier(uid, SITE_STUDY_UID_ROOT);
		break;
	case UID_Series:
		dcmGenerateUniqueIdentifier(uid, SITE_SERIES_UID_ROOT);
		break;
	case UID_Image:
		dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT);
		break;
	default:
		std::cout << "The type of generate unique id that is not supported! type :" << type << std::endl;
		break;
	}
	return std::string(uid);
}

std::string GetDataVR(DataVR_Type type)
{
	std::string vr = "";
	switch (type)
	{
	case DataVRType_CS:
		vr = "CS";
		break;
	case DataVRType_SH:
		vr = "SH";
		break;
	case DataVRType_LO:
		vr = "LO";
		break;
	case DataVRType_ST:
		vr = "ST";
		break;
	case DataVRType_LT:
		vr = "LT";
		break;
	case DataVRType_UT:
		vr = "UT";
		break;
	case DataVRType_AE:
		vr = "AE";
		break;
	case DataVRType_PN:
		vr = "PN";
		break;
	case DataVRType_UI:
		vr = "UI";
		break;
	case DataVRType_DA:
		vr = "DA";
		break;
	case DataVRType_TM:
		vr = "TM";
		break;
	case DataVRType_DT:
		vr = "DT";
		break;
	case DataVRType_AS:
		vr = "AS";
		break;
	case DataVRType_IS:
		vr = "IS";
		break;
	case DataVRType_DS:
		vr = "DS";
		break;
	case DataVRType_SS:
		vr = "SS";
		break;
	case DataVRType_US:
		vr = "US";
		break;
	case DataVRType_SL:
		vr = "SL";
		break;
	case DataVRType_UL:
		vr = "UL";
		break;
	case DataVRType_AT:
		vr = "AT";
		break;
	case DataVRType_FL:
		vr = "FL";
		break;
	case DataVRType_FD:
		vr = "FD";
		break;
	case DataVRType_OB:
		vr = "OB";
		break;
	case DataVRType_OW:
		vr = "OW";
		break;
	case DataVRType_OF:
		vr = "OF";
		break;
	case DataVRType_SQ:
		vr = "SQ";
		break;
	case DataVRType_UN:
		vr = "UN";
		break;
	default:
		break;
	}
	return vr;
}


std::chrono::system_clock::time_point StringtoDate(std::string date)
{
	std::string year = date.substr(0, 4);
	std::string month = date.substr(4, 2);
	std::string day = date.substr(6, 2);
	tm tmObj;
	tmObj.tm_year = std::atoi(year.c_str()) - 1900;
	tmObj.tm_mon = std::atoi(month.c_str()) - 1;
	tmObj.tm_mday = std::atoi(day.c_str());
	tmObj.tm_hour = std::atoi("0");
	tmObj.tm_min = std::atoi("0");
	tmObj.tm_sec = std::atoi("0");

	//std::time_t tt = mktime(&tmObj);
	return std::chrono::system_clock::from_time_t(mktime(&tmObj));
}

std::chrono::system_clock::time_point StringtoDateTime(std::string date, std::string time)
{
	std::string year = date.substr(0, 4);
	std::string month = date.substr(4, 2);
	std::string day = date.substr(6, 2);
	std::string hour = time.substr(0, 2);
	std::string min = time.substr(2, 2);
	std::string sec = time.substr(4, 2);

	tm tmObj;
	tmObj.tm_year = std::atoi(year.c_str()) - 1900;
	tmObj.tm_mon = std::atoi(month.c_str()) - 1;
	tmObj.tm_mday = std::atoi(day.c_str());
	tmObj.tm_hour = std::atoi(hour.c_str());
	tmObj.tm_min = std::atoi(min.c_str());
	tmObj.tm_sec = std::atoi(sec.c_str());

	size_t pos = time.find(".", 0);
	if (pos == std::string::npos)
	{
		return std::chrono::system_clock::from_time_t(std::mktime(&tmObj));
	}
	else
	{
		std::string strMS = time.substr(pos + 1, time.length() - pos - 1);
		long long ms = std::atoll(strMS.c_str());
		return (std::chrono::system_clock::from_time_t(std::mktime(&tmObj)) + std::chrono::microseconds(ms));
	}
}


std::string DatetoString(std::chrono::system_clock::time_point timePoint)
{
	std::time_t tt = std::chrono::system_clock::to_time_t(timePoint);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&tt), "%Y%m%d");
	return ss.str();
}

std::string TimetoString(std::chrono::system_clock::time_point timePoint)
{
	std::time_t tt = std::chrono::system_clock::to_time_t(timePoint);

	auto ms = timePoint.time_since_epoch();
	auto diff = std::chrono::duration_cast<std::chrono::microseconds>(ms).count();
	auto const mcsecs = diff % 1000000;

	std::stringstream ss;
	if (mcsecs == 0)
	{
		ss << std::put_time(std::localtime(&tt), "%H%M%S");
	}
	else
	{
		ss << std::put_time(std::localtime(&tt), "%H%M%S") << "." << mcsecs;
	}
	return ss.str();
}

DicomProcessor::DicomProcessor() : m_fileformat(&m_dataSet)
{
	Reset();
}


DicomProcessor::~DicomProcessor()
{
	FreePixelData();
}

void DicomProcessor::ReadDicom(std::string file)
{
	//PrivateData data;
	//data.GroupTag = 0x4001;
	//data.ElementTag = 0x1000;
	//PrivateDataInfo().push_back(data);

	if (!Read(file))
	{
		std::cout << "Read Dicom File Failed: " << file.c_str() << std::endl;
		return;
	}

	for (auto &item : PrivateDataInfo())
	{
		std::ios::fmtflags fmt(std::cout.flags());
		std::cout << "Private Data:(0x" << std::hex << item.GroupTag << ",0x" << item.ElementTag << ")" << item.Value << std::endl;
		std::cout.flags(fmt);
	}
}

void DicomProcessor::SaveDicom(std::string file)
{
	PatientInfo()->PatientID = "88888";
	StudyInfo()->StudyInstanceUID = GenerateUniqueId(UID_Study);
	SeriesInfo()->SeriesDateTime = std::chrono::system_clock::now();


	if (!Save(file))
	{
		std::cout << "Save Dicom File Failed: " << file.c_str() << std::endl;
		return;
	}
}

void DicomProcessor::Reset()
{
	m_patient.reset(new Patient());
	m_study.reset(new Study());
	m_series.reset(new Series());
	m_image.reset(new Image());
	m_privateData.clear();
	FreePixelData();
}

bool DicomProcessor::Read(std::string filePath)
{
	OFCondition status = m_fileformat.loadFile(filePath.c_str());
	if (!status.good())
	{
		std::cout << "Read dimcom file error£º" << status.text() << ",file: " << filePath << std::endl;
		return false;
	}
	//try
	//{
	//	GetPatient();
	//	GetStudy();
	//	GetSeries();
	//	GetImage();
	//	GetPrivateData();
	//}
	//catch (...)
	//{
	//	std::cout << "Get dimcom info error!" << std::endl;
	//	FreePixelData();
	//	return false;
	//}
	std::cout << "Read dimcom file succeed£¡" << std::endl;
	return true;
}

bool DicomProcessor::Save(std::string filePath)
{
	PrivateData data;
	data.GroupTag = 0x4001;
	data.ElementTag = 0x1000;
	data.Name = "private1";
	data.VR = GetDataVR(DataVRType_UT);
	data.Value = "Private Test...";
	PrivateDataInfo().push_back(data);

	try
	{
		FillPatient();
		FillStudy();
		FillSeries();
		FillImage();
		FillPrivateData();
	}
	catch (const std::exception&)
	{
		std::cout << "Fill dimcom info error!" << std::endl;
		return false;
	}

	OFCondition status = m_fileformat.saveFile(filePath.c_str());
	if (!status.good())
	{
		std::cout << "Save dimcom file error, file: " << filePath << std::endl;
		return false;
	}

	std::cout << "Save dimcom file succeed£¡" << std::endl;
	return true;
}

void DicomProcessor::MetaInfoTags()
{
	std::cout << "Meta Tag-----------------------------Meta Tag" << std::endl;
	DcmObject* item = m_fileformat.getMetaInfo()->nextInContainer(NULL);
	while (item)
	{
		DcmVR valueVR(item->getVR());
		DcmTag tag(item->getTag());

		std::cout << item->getTag().toString().c_str() << "    " << valueVR.getVRName() << "    " << tag.getTagName() << std::endl;
		item = m_fileformat.getMetaInfo()->nextInContainer(item);
	}
}

void DicomProcessor::DataSetTags()
{
	std::cout << "DataSet Tag------------------------------DataSet Tag" << std::endl;
	DcmObject* item = m_fileformat.getDataset()->nextInContainer(NULL);
	while (item)
	{
		DcmVR valueVR(item->getVR());
		DcmTag tag(item->getTag());
		std::cout << item->getTag().toString().c_str() << "    " << valueVR.getVRName() << "    " << tag.getTagName() << std::endl;
		item = m_fileformat.getDataset()->nextInContainer(item);
	}
}


void DicomProcessor::GetPatient()
{
	GetInfo(DCM_PatientID, m_patient->PatientID);
	GetInfo(DCM_PatientName, m_patient->PatientsName);
}

void DicomProcessor::GetStudy()
{
	GetInfo(DCM_StudyInstanceUID, m_study->StudyInstanceUID);
	if (m_study->StudyInstanceUID.empty())
	{
		std::cout << "Get Tag DCM_StudyInstanceUID Error£¡" << std::endl;
	}
}

void DicomProcessor::GetSeries()
{
	//GetDateTimeInfo(DCM_SeriesDate, DCM_SeriesTime, m_series->SeriesDateTime);
	GetInfo(DCM_BodyPartExamined, m_series->BodyPartExamined);
}

void DicomProcessor::GetImage()
{
	GetInfo(DCM_SamplesPerPixel, m_image->SamplesPerPixel);
	GetInfo(DCM_PhotometricInterpretation, m_image->PhotometricInterpretation);
	GetInfo(DCM_Rows, m_image->Rows);
	GetInfo(DCM_Columns, m_image->Columns);
	GetInfo(DCM_BitsAllocated, m_image->BitsAllocated);
	GetInfo(DCM_BitsStored, m_image->BitsStored);
	GetInfo(DCM_HighBit, m_image->HighBit);
	GetInfo(DCM_PixelRepresentation, m_image->PixelRepresentation);

	uint pixelByteCount = (m_image->BitsAllocated <= 8) ? 1 : 2;
	ulong dataLength = m_image->Rows * m_image->Columns * pixelByteCount * m_image->SamplesPerPixel;

	DcmElement* element = NULL;
	OFCondition status = m_fileformat.getDataset()->findAndGetElement(DCM_PixelData, element);
	if (!status.good())
	{
		std::cout << "Get pixel data element error:" << status.text() << std::endl;
	}
	else
	{
		//std::cout << "Pixel data element's length:" << element->getLength() << std::endl;

		unsigned short* pData = NULL;
		status = element->getUint16Array(pData);
		if (!status.good())
		{
			std::cout << "Get pixel data array error:" << status.text() << std::endl;
			return;
		}
		FreePixelData();
		m_image->PixelData = (uint16_t *)malloc(dataLength);
		memcpy(m_image->PixelData, pData, dataLength);
	}
}

void DicomProcessor::GetPrivateData()
{
	DcmTagKey tagKey;
	OFString value;
	DcmElement* element = NULL;

	for (auto &data : m_privateData)
	{
		tagKey.set((Uint16)data.GroupTag, (Uint16)data.ElementTag);
		value.clear();
		element = NULL;
		OFCondition status = m_fileformat.getDataset()->findAndGetElement(tagKey, element);

		if (!status.good())
		{
			std::ios::fmtflags fmt(std::cout.flags());
			std::cout << "Get private Tag(" << std::hex << data.GroupTag << "," << data.ElementTag << ") error:" << status.text() << std::endl;
			std::cout.flags(fmt);
		}
		else
		{
			status = element->getOFString(value, 0);
			if (!status.good())
			{
				std::ios::fmtflags fmt(std::cout.flags());
				std::cout << "Get private Tag(" << std::hex << data.GroupTag << "," << data.ElementTag << ") error:" << status.text() << std::endl;
				std::cout.flags(fmt);
			}
			else
			{
				data.Value = std::string(value.c_str());
				DcmVR valueVR(element->getVR());
				data.VR = std::string(valueVR.getVRName());
			}
		}
	}
}


void DicomProcessor::FillPatient()
{
	FillInfo(DCM_PatientID, m_patient->PatientID);
	FillInfo(DCM_PatientName, m_patient->PatientsName);
}

void DicomProcessor::FillStudy()
{
	StudyInfo()->StudyInstanceUID = GenerateUniqueId(UID_Study);

	if (m_study->StudyInstanceUID.empty())
	{
		std::cout << "Study instanceUID is empty!" << std::endl;
	}

	FillInfo(DCM_StudyInstanceUID, m_study->StudyInstanceUID);
}

void DicomProcessor::FillSeries()
{
	m_series->SeriesDateTime = std::chrono::system_clock::now();
	FillInfo(DCM_SeriesDate, DatetoString(m_series->SeriesDateTime));
	FillInfo(DCM_SeriesTime, TimetoString(m_series->SeriesDateTime));
}

void DicomProcessor::FillImage()
{
	m_image->SamplesPerPixel = 1;
	FillInfo(DCM_SamplesPerPixel, std::to_string(m_image->SamplesPerPixel));

	if (m_image->PhotometricInterpretation.empty())
	{
		std::cout << "No PhotometricInterpretation Info in Fill image Pixel" << std::endl;
	}

	FillInfo(DCM_PhotometricInterpretation, std::string("MONOCHROME2"));
	//FillInfo(DCM_PhotometricInterpretation, m_image->PhotometricInterpretation);
	FillInfo(DCM_Rows, std::to_string(m_image->Rows));
	FillInfo(DCM_Columns, std::to_string(m_image->Columns));
	FillInfo(DCM_BitsAllocated, std::to_string(16));
	//FillInfo(DCM_BitsAllocated, std::to_string(m_image->BitsAllocated));
	FillInfo(DCM_BitsStored, std::to_string(16));
	m_image->BitsAllocated = 16;
	//FillInfo(DCM_BitsStored, std::to_string(m_image->BitsStored));
	FillInfo(DCM_HighBit, std::to_string(15));
	//FillInfo(DCM_HighBit, std::to_string(m_image->HighBit));
	FillInfo(DCM_PixelRepresentation, std::to_string(0));
	//FillInfo(DCM_PixelRepresentation, std::to_string(m_image->PixelRepresentation));

	uint pixelByteCount = (m_image->BitsAllocated <= 8) ? 1 : 2;
	//uint pixelByteCount = 1;
	ulong dataLength = m_image->Rows * m_image->Columns * pixelByteCount * m_image->SamplesPerPixel;

	if (m_image->PixelData != NULL)
	{
		OFCondition status = m_fileformat.getDataset()->putAndInsertUint16Array(DCM_PixelData, m_image->PixelData, dataLength / pixelByteCount);
		if (!status.good())
		{
			std::cout << "Fill pixel data error:" << status.text() << std::endl;
		}
	}
}

void DicomProcessor::FillPrivateData()
{
	for (auto &data : m_privateData)
	{
		if (!data.VR.empty())
		{
			RegisterPrivateTags(data);
			OFCondition status = m_fileformat.getDataset()->putAndInsertString(DcmTag((Uint16)data.GroupTag, (Uint16)data.ElementTag, PRIVATE_GROUP_NAME), data.Value.c_str());
			if (!status.good())
			{
				std::cout << "Fill private Tag(" << data.GroupTag << "," << data.ElementTag << ") error:" << status.text();
			}
		}
	}
	m_privateData.clear();
}

void DicomProcessor::RegisterPrivateTags(PrivateData data)
{
	DcmDataDictionary &dict = dcmDataDict.wrlock();

	if ((data.VR.compare("OB") == 0) || (data.VR.compare("FL") == 0) || (data.VR.compare("FD") == 0))
	{
		dict.addEntry(new DcmDictEntry((Uint16)data.GroupTag, (Uint16)data.ElementTag, EVR_UT, data.Name.c_str(), 1, 1, "private", OFTrue, PRIVATE_GROUP_NAME));
	}
	else
	{
		dict.addEntry(new DcmDictEntry((Uint16)data.GroupTag, (Uint16)data.ElementTag, DcmVR(data.VR.c_str()), data.Name.c_str(), 1, 1, "private", OFTrue, PRIVATE_GROUP_NAME));
	}
	dcmDataDict.wrunlock();
}


void DicomProcessor::GetInfo(DcmTagKey tagKey, std::string &info)
{
	OFString ofData;
	OFCondition status = m_fileformat.getDataset()->findAndGetOFString(tagKey, ofData);
	if (!status.good())
	{
		std::cout << "Get Tag(" << tagKey.toString().c_str() << ") error: " << status.text() << std::endl;
	}
	info = ofData.c_str();
}

void DicomProcessor::GetInfo(DcmTagKey tagKey, unsigned int &info)
{
	std::string strInfo;
	GetInfo(tagKey, strInfo);
	try
	{
		info = std::atoi(strInfo.c_str());
	}
	catch (...)
	{
		std::cout << "Get Tag(" << tagKey.toString().c_str() << ") error(String to UInt)£¡" << std::endl;
	}
}

void DicomProcessor::GetDateTimeInfo(DcmTagKey tagDateKey, DcmTagKey tagTimeKey, std::chrono::system_clock::time_point &info)
{
	std::string date;
	std::string time;
	GetInfo(tagDateKey, date);
	GetInfo(tagTimeKey, time);
	info = StringtoDateTime(date, time);
}

void DicomProcessor::FillInfo(DcmTagKey tagKey, std::string &info)
{
	OFCondition status = m_fileformat.getDataset()->putAndInsertString(tagKey, info.c_str());
	if (!status.good())
	{
		std::cout << "Fill Tag(" << tagKey.toString().c_str() << ") error: " << status.text() << std::endl;
	}
}

void DicomProcessor::FreePixelData()
{
	if (m_image->PixelData != NULL)
	{
		//std::cout << "Free pixel data of Dicom Handler" << std::endl;
		free(m_image->PixelData);
		m_image->PixelData = NULL;
	}
}
