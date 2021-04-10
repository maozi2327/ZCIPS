#include "stdafx.h"
#include "tipdialog.h"

TipDialog::TipDialog(QDialog *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

TipDialog::~TipDialog()
{

}

void TipDialog::setTip(const std::vector<QString>& _tip)
{
	ui.tipListWidget->clear();

	for (auto& tip : _tip)
		ui.tipListWidget->addItem(tip);

	show();
}

void TipDialog::clearTip()
{
	hide();
}
