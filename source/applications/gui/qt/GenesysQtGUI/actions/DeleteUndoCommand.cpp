#include "DeleteUndoCommand.h"

DeleteUndoCommand::DeleteUndoCommand(GraphicalModelComponent *gmc, QGraphicsScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent), myGraphicsScene(scene) {
     std::cout << "oi" << std::endl;
//    QList<QGraphicsItem *> list = myGraphicsScene->selectedItems();
//    list.first()->setSelected(false);
//    myDiagramItem = static_cast<DiagramItem *>(list.first());
//    setText(QObject::tr("Delete %1")
//                .arg(createCommandString(myDiagramItem, myDiagramItem->pos())));
}

DeleteUndoCommand::~DeleteUndoCommand() {
    delete this;
}

void DeleteUndoCommand::undo() {
     std::cout << "oi" << std::endl;
//    myGraphicsScene->addItem(myDiagramItem);
//    myGraphicsScene->update();
}

void DeleteUndoCommand::redo() {
      std::cout << "oi" << std::endl;
//    myGraphicsScene->removeItem(myDiagramItem);
}
