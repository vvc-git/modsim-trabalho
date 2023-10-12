#ifndef MOVEUNDOCOMMAND_H
#define MOVEUNDOCOMMAND_H

#include <QUndoCommand>
#include "ModelGraphicsScene.h"
#include "graphicals/GraphicalModelComponent.h"

class MoveUndoCommand : public QUndoCommand
{
public:
    enum { Id = 1234 };

    MoveUndoCommand(QList<GraphicalModelComponent*> gmc, ModelGraphicsScene *scene, QList<QPointF> &oldPos, QList<QPointF> &newPos, QUndoCommand *parent = nullptr);
    ~MoveUndoCommand();

    void undo() override;
    void redo() override;

    bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

private:
    QList<GraphicalModelComponent*> _myGraphicalModelComponent;
    ModelGraphicsScene *_myGraphicsScene;
    QList<QPointF> _myOldPos;
    QList<QPointF> _myNewPos;
    bool _firstExecution;
};

#endif // MOVEUNDOCOMMAND_H
