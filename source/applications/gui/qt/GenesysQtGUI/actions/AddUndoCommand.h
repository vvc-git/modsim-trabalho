#ifndef ADDUNDOCOMMAND_H
#define ADDUNDOCOMMAND_H

#include <QUndoCommand>
#include "ModelGraphicsView.h"
#include "ModelGraphicsScene.h"
#include "graphicals/GraphicalModelComponent.h"


class AddUndoCommand: public QUndoCommand {
public:
    AddUndoCommand(GraphicalModelComponent *gmc, ModelGraphicsScene *scene, QUndoCommand *parent = nullptr);
    ~AddUndoCommand();

    void undo();
    void redo();

private:
    GraphicalModelComponent *myGraphicalModelComponent;
    ModelGraphicsScene *myGraphicsScene;
    QPointF initialPosition;
    bool firstExecution;
};

#endif // ADDUNDOCOMMAND_H
