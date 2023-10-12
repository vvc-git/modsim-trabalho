#include "MoveUndoCommand.h"
#include "ModelGraphicsScene.h"

MoveUndoCommand::MoveUndoCommand(GraphicalModelComponent *gmc, ModelGraphicsScene *scene, const QPointF &oldPos, const QPointF &newPos, QUndoCommand *parent)
    : QUndoCommand(parent), _myGraphicalModelComponent(gmc), _myGraphicsScene(scene), _myOldPos(oldPos), _myNewPos(newPos) {

    _firstExecution = true;
}

MoveUndoCommand::~MoveUndoCommand() {}

void MoveUndoCommand::undo() {

        _myGraphicalModelComponent->setPos(_myOldPos);
        _myGraphicsScene->update();

        std::string position = "position=(x=" + std::to_string(_myOldPos.x()) + ", y=" + std::to_string(_myOldPos.y()) + ")";

        setText(QObject::tr("Move %1")
                    .arg(QString::fromStdString("name=" + _myGraphicalModelComponent->getComponent()->getName() + ", " + position)));


}

void MoveUndoCommand::redo() {
    if (!_firstExecution) {
        _myGraphicalModelComponent->setPos(_myNewPos);
        _myGraphicsScene->update();

        std::string position = "position=(x=" + std::to_string(_myNewPos.x()) + ", y=" + std::to_string(_myNewPos.y()) + ")";

        setText(QObject::tr("Move %1")
                    .arg(QString::fromStdString("name=" + _myGraphicalModelComponent->getComponent()->getName() + ", " + position)));
    }
    _firstExecution = false;
}

bool MoveUndoCommand::mergeWith(const QUndoCommand *command) {
    const MoveUndoCommand *moveCommand = static_cast<const MoveUndoCommand *>(command);
    GraphicalModelComponent *item = moveCommand->_myGraphicalModelComponent;

    if (_myGraphicalModelComponent != item)
        return false;

    _myNewPos = item->scenePos();

    std::string position = "newPosition=(x=" + std::to_string(_myNewPos.x()) + ", y=" + std::to_string(_myNewPos.y()) + ")";

    setText(QObject::tr("Move %1")
                .arg(QString::fromStdString("name=" + _myGraphicalModelComponent->getComponent()->getName() + ", " + position)));

    return true;
}
