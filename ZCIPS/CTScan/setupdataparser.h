#pragma once

#include <QObject>
#include "../Public/lib/tinyxml2.h"
#include "../Public/headers/SetupData.h"
struct SetupData;
class SetupDataParser : public QObject
{
	Q_OBJECT

private:
	QString d_setupXMLName;
	QString d_setupDatName;
	std::unique_ptr<tinyxml2::XMLDocument> d_XMLDoc;
	SetupData* d_setupData;
	bool parseLineDetSection(tinyxml2::XMLElement* _element);
	bool parseLineDetData(tinyxml2::XMLElement* _element, int _number);
	bool parsePanelDetSection(tinyxml2::XMLElement* _element);
	bool parsePanelDetData(tinyxml2::XMLElement* _element, int _number);
	bool parsekVRaySection(tinyxml2::XMLElement* _element);
	bool parsekVRayData(tinyxml2::XMLElement* _element, int _number, kVRayData& _kvRayData);
	bool parseAcceleratorSection(tinyxml2::XMLElement* _element);
	bool parseAcceleratorData(tinyxml2::XMLElement* _element, int _number, AcceleratorData& _accData);
	bool parseScanModeSection(tinyxml2::XMLElement* _element);
	bool parseAxisDefinition(tinyxml2::XMLElement* _element);
	bool parseCT2Data(tinyxml2::XMLElement* _element);
	bool parseCT3Data(tinyxml2::XMLElement* _element);
	bool parseDRData(tinyxml2::XMLElement* _element);
	bool parseConeScanData(tinyxml2::XMLElement* _element);
	bool parseConeJointRotScanData(tinyxml2::XMLElement* _element);
public:
	SetupDataParser(SetupData* _setupData, QObject *parent = Q_NULLPTR);
	~SetupDataParser();
	bool parseSetupXMLFile();
	bool saveSetupData();
};
