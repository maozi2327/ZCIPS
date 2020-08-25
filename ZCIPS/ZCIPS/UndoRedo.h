#pragma once
#include <QUndoCommand>
#include "ImageInstance.h"

/*********************** 撤销和重做类 *****************
描述：包含撤销和重做的所有操作
创建时间：2020.08.13
******************************************************/
class ImageInstance;
class UndoRedoCommand :public QUndoCommand
{
public:
	UndoRedoCommand();
	~UndoRedoCommand();
	void redo() override;
	void undo() override;
private:
	QPair<QString, QSharedPointer<ImageInstance>> PreInstance;
	QPair<QString, QSharedPointer<ImageInstance>> CurInstance;
};
