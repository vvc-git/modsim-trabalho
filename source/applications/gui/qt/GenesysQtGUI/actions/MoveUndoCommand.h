#ifndef MOVEUNDOCOMMAND_H
#define MOVEUNDOCOMMAND_H

#include <QUndoCommand>
#include "ModelGraphicsScene.h"
#include "graphicals/GraphicalModelComponent.h"

class MoveUndoCommand : public QUndoCommand
{
public:
    enum { Id = 1234 };

    MoveUndoCommand(GraphicalModelComponent *gmc, ModelGraphicsScene *scene, const QPointF &oldPos, const QPointF &newPos, QUndoCommand *parent = nullptr);
    ~MoveUndoCommand();

    void undo() override;
    void redo() override;

    bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

private:
    GraphicalModelComponent *_myGraphicalModelComponent;
    ModelGraphicsScene *_myGraphicsScene;
    QPointF _myOldPos;
    QPointF _myNewPos;
};

#endif // MOVEUNDOCOMMAND_H
