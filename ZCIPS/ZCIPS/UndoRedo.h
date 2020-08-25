#pragma once
#include <QUndoCommand>
/*********************** ������������ *****************
�������������������������в���
����ʱ�䣺2020.08.13
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
