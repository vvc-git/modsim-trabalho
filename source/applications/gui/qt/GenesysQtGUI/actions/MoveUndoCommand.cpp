#include "MoveUndoCommand.h"

MoveUndoCommand::MoveUndoCommand(GraphicalModelComponent *gmc, QGraphicsScene *scene, const QPointF &oldPos, QUndoCommand *parent)
    : QUndoCommand(parent), myGraphicalModelComponent(gmc), myOldPos(oldPos) {
    //newPos(diagramItem->pos())

    std::cout << "oi" << std::endl;
}

MoveUndoCommand::~MoveUndoCommand() {
    delete this;
}

void MoveUndoCommand::undo() {
    std::cout << "oi" << std::endl;
//    myDiagramItem->setPos(myOldPos);
//    myDiagramItem->scene()->update();
//    setText(QObject::tr("Move %1")
//                .arg(createCommandString(myDiagramItem, newPos)));
}

void MoveUndoCommand::redo() {
    std::cout << "oi" << std::endl;
//    myDiagramItem->setPos(newPos);
//    setText(QObject::tr("Move %1")
//                .arg(createCommandString(myDiagramItem, newPos)));
}

bool MoveUndoCommand::mergeWith(const QUndoCommand *command) {
    std::cout << "oi" << std::endl;
//    const MoveUndoCommand *MoveUndoCommand = static_cast<const MoveUndoCommand *>(command);
//    DiagramItem *item = MoveUndoCommand->myDiagramItem;

//    if (myDiagramItem != item)
//        return false;

//    newPos = item->pos();
//    setText(QObject::tr("Move %1")
//                .arg(createCommandString(myDiagramItem, newPos)));

//    return true;
}
