#pragma once
#include <QObject>

int messageBox(const QString& text, const QString& infoText);
int messageBoxOkCancel(const QString& text, const QString& infoText);
int messageBoxOkCancelDiscard(const QString& text, const QString& infoText);
