#include "MoveUndoCommand.h"
#include "ModelGraphicsScene.h"

MoveUndoCommand::MoveUndoCommand(QList<GraphicalModelComponent*> gmc, ModelGraphicsScene *scene, QList<QPointF> &oldPos, QList<QPointF> &newPos, QUndoCommand *parent)
    : QUndoCommand(parent), _myGraphicalModelComponent(gmc), _myGraphicsScene(scene), _myOldPos(oldPos), _myNewPos(newPos) {

    _firstExecution = true;
}

MoveUndoCommand::~MoveUndoCommand() {}

void MoveUndoCommand::undo() {

    for (int i = 0; i < _myGraphicalModelComponent.size(); i++) {
        GraphicalModelComponent *item = _myGraphicalModelComponent[i];
        QPointF oldPos = _myOldPos[i];

        item->setPos(oldPos);
        _myGraphicsScene->update();

        std::string position = "position=(x=" + std::to_string(oldPos.x()) + ", y=" + std::to_string(oldPos.y()) + ")";

        setText(QObject::tr("Move %1")
                    .arg(QString::fromStdString("name=" + item->getComponent()->getName() + ", " + position)));
    }
}

void MoveUndoCommand::redo() {
    if (!_firstExecution) {
        for (int i = 0; i < _myGraphicalModelComponent.size(); i++) {
            GraphicalModelComponent *item = _myGraphicalModelComponent[i];
            QPointF newPos = _myNewPos[i];

            item->setPos(newPos);
            _myGraphicsScene->update();

            std::string position = "position=(x=" + std::to_string(newPos.x()) + ", y=" + std::to_string(newPos.y()) + ")";

            setText(QObject::tr("Move %1")
                        .arg(QString::fromStdString("name=" + item->getComponent()->getName() + ", " + position)));
        }
    }
    _firstExecution = false;
}

bool MoveUndoCommand::mergeWith(const QUndoCommand *command) {
//    for (int i = 0; i < _myGraphicalModelComponent.size(); i++) {
//        const MoveUndoCommand *moveCommand = static_cast<const MoveUndoCommand *>(command);
//        GraphicalModelComponent *itemCommand = moveCommand->_myGraphicalModelComponent[i];

//        if (_myGraphicalModelComponent[i] != itemCommand)
//            return false;

//        _myNewPos[i] = itemCommand->scenePos();

//        std::string position = "newPosition=(x=" + std::to_string(_myNewPos[i].x()) + ", y=" + std::to_string(_myNewPos[i].y()) + ")";

//        setText(QObject::tr("Move %1")
//                    .arg(QString::fromStdString("name=" + _myGraphicalModelComponent[i]->getComponent()->getName() + ", " + position)));
//    }

    return true;
}
