#ifndef ADDUNDOCOMMAND_H
#define ADDUNDOCOMMAND_H

#include <QUndoCommand>
#include "ModelGraphicsScene.h"
#include "graphicals/GraphicalModelComponent.h"


class AddUndoCommand: public QUndoCommand {
public:
    AddUndoCommand(GraphicalModelComponent *gmc, ModelGraphicsScene *scene, QUndoStack *stack, QUndoCommand *parent = nullptr);
    ~AddUndoCommand();

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

#endif // ADDUNDOCOMMAND_H
