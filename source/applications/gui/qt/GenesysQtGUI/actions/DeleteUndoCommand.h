#ifndef DELETEUNDOCOMMAND_H
#define DELETEUNDOCOMMAND_H

#include <QUndoCommand>
#include "ModelGraphicsScene.h"
#include "graphicals/GraphicalModelComponent.h"

class DeleteUndoCommand : public QUndoCommand
{
public:
    explicit DeleteUndoCommand(GraphicalModelComponent *gmc, ModelGraphicsScene *scene, QUndoStack *stack, QUndoCommand *parent = nullptr);
    ~DeleteUndoCommand();

    void undo() override;
    void redo() override;
    GraphicalModelComponent* getMyGraphicalModelComponent() const;

private:
    GraphicalModelComponent *_myGraphicalModelComponent;
    ModelGraphicsScene *_myGraphicsScene;
    QPointF _initialPosition;
    bool _firstExecution;
    QUndoStack *_myUndoStack = nullptr;
};

#endif // DELETEUNDOCOMMAND_H
