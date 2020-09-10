#include "stdafx.h"
#include "msglistbox.h"
#include <string>
#include <regex>

MsgListBox::MsgListBox(QString& _logFileName, QWidget *parent)
	: QWidget(parent)
{
	CIULog::Init(_logFileName);
	ui.setupUi(this);
}

MsgListBox::~MsgListBox()
{
}
QString extractMsg(QString& _msg)
{
	_msg.insert(1, QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + " ");
	std::string text = _msg.toLocal8Bit();
	std::string pattern("\\[([^\\]]+)\\]");    //只能达到匹配成[xxxx]的程度
	//std::string pattern("(?=\\[)[^\\]]*");   //只能达到匹配成[xxxx的程度
	std::regex r(pattern);
	std::smatch result;
	QString output;

	if (std::regex_search(text, result, r))
	{
		std::string temp = result.str();
		std::string tempp(temp, 1, temp.size() - 2);
		output = QString::fromLocal8Bit(tempp.data());
	}

	return output;
}

void MsgListBox::logError(QString& _msg)
{
	ui.listWidget->addItem(extractMsg(_msg));
	CIULog::Log(LOG_LEVEL::LOG_LEVEL_ERROR, _msg);
	show();
}

void MsgListBox::logInfo(QString & _msg)
{
	ui.listWidget->addItem(extractMsg(_msg));
	CIULog::Log(LOG_LEVEL::LOG_LEVEL_INFO, _msg);
	show();
}

void MsgListBox::logBug(QString & _msg)
{
	ui.listWidget->addItem(extractMsg(_msg));
	CIULog::Log(LOG_LEVEL::LOG_LEVEL_BUG, _msg);
	show();
}

void MsgListBox::closeEvent(QCloseEvent * event)
{
	hide();
}

