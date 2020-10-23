#pragma once

#include <QObject>

class ct3AddDialog;
class CT3TemplateDialog;

class Ct3TemplateManager : public QObject
{
	Q_OBJECT

public:
	Ct3TemplateManager(QObject *parent);
	~Ct3TemplateManager();

private:
	CT3TemplateDialog* d_ct3TemplateDialog;
};
