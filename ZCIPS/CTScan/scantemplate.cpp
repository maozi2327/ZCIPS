#include "stdafx.h"
#include "scantemplate.h"

bool operator==(const Ct3TemplateData& t1, const Ct3TemplateData& t2)
{
	bool flag1 =
		t1.Name == t2.Name &&
		t1.MutilayerOrEqualLayer == t2.MutilayerOrEqualLayer &&
		t1.Matrix == t2.Matrix &&
		t1.View == t2.View &&
		t1.SampleTime == t2.SampleTime &&
		t1.Orientation == t2.Orientation;

	if (flag1)
	{
		bool flag2;

		if (t1.MutilayerOrEqualLayer == EQUALLAYER)
			flag2 =
			t1.LayerNumber == t2.LayerNumber &&
			t1.layerSpace == t2.layerSpace &&
			t1.LayerPos == t2.LayerPos;
		else if (t1.MutilayerOrEqualLayer == MULTILAYER)
			flag2 = t1.LayerPos == t2.LayerPos;

		return flag2;
	}

	return false;
}
bool operator<(const Ct3TemplateData& t1, const Ct3TemplateData& t2)
{
	return t1.Name < t2.Name;
}
