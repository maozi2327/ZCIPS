#pragma once
#include <set>
const unsigned char EQUALLAYER = 1;
const unsigned char MULTILAYER = 0;
const unsigned int FILEHEAD = 0XAAAAAAA3;

struct Ct3TemplateData
{
	QString Name;
	unsigned char MutilayerOrEqualLayer;
	unsigned int Matrix;
	unsigned int View;
	unsigned int SampleTime;
	float Orientation;
	unsigned int LayerNumber;  //只用作等层距扫描计数
	float layerSpace;
	std::set<float> LayerPos;
};
bool operator==(const Ct3TemplateData& t1, const Ct3TemplateData& t2);
bool operator<(const Ct3TemplateData& t1, const Ct3TemplateData& t2);

