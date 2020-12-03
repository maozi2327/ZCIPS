#include "stdafx.h"
#include "machineType.h"

extern const std::map<QString, Axis> AxisNameMap =
{
	{QString::fromLocal8Bit("ObjRotation"), Axis::objRotation},
	{ QString::fromLocal8Bit("ObjTranslation"), Axis::objTranslation },
	{ QString::fromLocal8Bit("ObjRadial"), Axis::objRadial },
	{ QString::fromLocal8Bit("RayLayer"), Axis::rayLayer },
	{ QString::fromLocal8Bit("DetLayer"), Axis::detLayer },
	{ QString::fromLocal8Bit("DetTranslation"), Axis::detTranslation },
	{ QString::fromLocal8Bit("DetRadial"), Axis::detRadial }
};

extern const std::map<Axis, AxisData> AxisDataMap
{
	{Axis::objRotation, AxisData{QString::fromLocal8Bit("�ֶ�"), "objRotaion"}},
	{Axis::objTranslation, AxisData{QString::fromLocal8Bit("����ƽ��"), "objTranslation"}},
	{Axis::objRadial, AxisData{QString::fromLocal8Bit("��������"), "objRadial"}},
	{Axis::rayLayer, AxisData{QString::fromLocal8Bit("�ֲ�1"), "rayLayer"}},
	{Axis::detLayer, AxisData{QString::fromLocal8Bit("�ֲ�2"), "detLayer"}},
	{Axis::detTranslation, AxisData{QString::fromLocal8Bit("̽��ƽ��"), "detTranslation"}},
	{Axis::detRadial, AxisData{QString::fromLocal8Bit("̽�⾶��"), "detRadial"}}
};

extern const std::map<Axis, AxisPosEnum> AxisNamePosMap
{
	{Axis::objRotation, AxisPosEnum::objRotation},
	{Axis::objTranslation, AxisPosEnum::objTranslation},
	{Axis::objRadial, AxisPosEnum::objRadial},
	{Axis::rayLayer, AxisPosEnum::rayLayer},
	{Axis::detLayer, AxisPosEnum::detLayer},
	{Axis::detTranslation, AxisPosEnum::detTranslation},
	{Axis::detRadial, AxisPosEnum::detRadial}
};

extern const std::map<Axis, AxisZeroEnum> AxisNameZeroMap
{
	{Axis::objRotation, AxisZeroEnum::objRotation},
	{Axis::objTranslation, AxisZeroEnum::objTranslation},
	{Axis::objRadial, AxisZeroEnum::objRadial},
	{Axis::rayLayer, AxisZeroEnum::rayLayer},
	{Axis::detLayer, AxisZeroEnum::detLayer},
	{Axis::detTranslation, AxisZeroEnum::detTranslation},
	{Axis::detRadial, AxisZeroEnum::detRadial}
};