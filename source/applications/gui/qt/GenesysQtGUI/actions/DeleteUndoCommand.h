#ifndef DELETEUNDOCOMMAND_H
#define DELETEUNDOCOMMAND_H

#include <QUndoCommand>
#include "graphicals/GraphicalModelComponent.h"

class DeleteUndoCommand : public QUndoCommand
{
public:
    explicit DeleteUndoCommand(GraphicalModelComponent *gmc, QGraphicsScene *scene, QUndoCommand *parent = nullptr);
    ~DeleteUndoCommand();

    void undo() override;
    void redo() override;

private:
    GraphicalModelComponent *myGraphicalModelComponent;
    QGraphicsScene *myGraphicsScene;
};

#endif // DELETEUNDOCOMMAND_H
