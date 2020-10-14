#include "stdafx.h"
#include "messagebox.h"

int messageBox(const QString& text, const QString& infoText)
{
	QMessageBox msgBox;
	msgBox.setText(text);
	msgBox.setInformativeText(infoText);
	msgBox.setStandardButtons(QMessageBox::Ok);
	return msgBox.exec();
}

int messageBoxOkCancel(const QString& text, const QString& infoText)
{
	QMessageBox msgBox;
	msgBox.setText(text);
	msgBox.setInformativeText(infoText);
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	return msgBox.exec();
}

int messageBoxOkCancelDiscard(const QString& text, const QString& infoText)
{
	QMessageBox msgBox;
	msgBox.setText(text);
	msgBox.setInformativeText(infoText);
	msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard);
	return msgBox.exec();
}