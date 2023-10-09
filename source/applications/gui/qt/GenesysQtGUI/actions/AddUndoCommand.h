#ifndef ADDUNDOCOMMAND_H
#define ADDUNDOCOMMAND_H

#include <QString>
#include <QUndoCommand>
#include <ModelGraphicsScene.h>
#include <ModelGraphicsView.h>
#include "graphicals/GraphicalModelComponent.h"


class AddUndoCommand: public QUndoCommand {
public:
    AddUndoCommand(GraphicalModelComponent *gmc, ModelGraphicsScene *scene, QUndoCommand *parent = nullptr);
    ~AddUndoCommand();

    void undo();
    void redo();

private:
    GraphicalModelComponent *myGraphicalModelComponent;
    GraphicalModelComponent *copyMyGraphicalModelComponent;
    ModelGraphicsScene *myGraphicsScene;
    QPointF initialPosition;
    bool first;
};

#endif // ADDUNDOCOMMAND_H
