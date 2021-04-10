#include "stdafx.h"
#include "colimatezeroajustdialog.h"
#include "leisaibac332e.h"
#include <vector>

int ColimateZeroAjustDialog::d_dialogCount = 0;

ColimateZeroAjustDialog::ColimateZeroAjustDialog(LeiSaiBac332E* _leiSai, QWidget *parent)
	: QDialog(parent), d_leisai(_leiSai)
{
	ui.setupUi(this);
	std::vector<float> zeros;
	d_leisai->getHomePosition(zeros);
	ui.rayHEdit->setText(QString::number(zeros[0], 'f', 1));
	ui.rayVEdit->setText(QString::number(zeros[1], 'f', 1));
	ui.detHEdit->setText(QString::number(zeros[2], 'f', 1));
	ui.detVEdit->setText(QString::number(zeros[3], 'f', 1));
	ui.layerThicknessEdit->setText(QString::number(zeros[4], 'f', 1));
	++d_dialogCount;
}

ColimateZeroAjustDialog::~ColimateZeroAjustDialog()
{
	--d_dialogCount;
}

int ColimateZeroAjustDialog::getDialogCount()
{
	return d_dialogCount;
}

void ColimateZeroAjustDialog::on_saveButton_clicked()
{
	std::vector<float> zeros(5);
	zeros[0] = ui.rayHEdit->text().toFloat();
	zeros[1] = ui.rayVEdit->text().toFloat();
	zeros[2] = ui.detHEdit->text().toFloat();
	zeros[3] = ui.detVEdit->text().toFloat();
	zeros[4] = ui.layerThicknessEdit->text().toFloat();
	d_leisai->setHomePosition(zeros);
}
