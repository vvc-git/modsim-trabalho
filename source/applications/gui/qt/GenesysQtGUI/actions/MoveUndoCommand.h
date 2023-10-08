#ifndef MOVEUNDOCOMMAND_H
#define MOVEUNDOCOMMAND_H

#include <QUndoCommand>
#include "graphicals/GraphicalModelComponent.h"

class MoveUndoCommand : public QUndoCommand
{
public:
    enum { Id = 1234 };

    MoveUndoCommand(GraphicalModelComponent *gmc, QGraphicsScene *scene, const QPointF &oldPos, QUndoCommand *parent = nullptr);
    ~MoveUndoCommand();

    void undo() override;
    void redo() override;

    bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

private:
    GraphicalModelComponent *myGraphicalModelComponent;
    QPointF myOldPos;
    QPointF newPos;
};

#endif // MOVEUNDOCOMMAND_H
