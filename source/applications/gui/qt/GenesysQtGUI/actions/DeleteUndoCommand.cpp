#include "AddUndoCommand.h"
#include "DeleteUndoCommand.h"
#include "ModelGraphicsView.h"

DeleteUndoCommand::DeleteUndoCommand(GraphicalModelComponent *gmc, ModelGraphicsScene *scene, QUndoStack* stack, QUndoCommand *parent)
    : QUndoCommand(parent), _myGraphicalModelComponent(gmc), _myGraphicsScene(scene), _myUndoStack(stack) {
    _firstExecution = true;
    _initialPosition = QPointF(gmc->scenePos().x(), gmc->scenePos().y() + gmc->getHeight()/2);

    std::string position = "position=(x=" + std::to_string(gmc->scenePos().x()) + ", y=" + std::to_string(gmc->scenePos().y()) + ")";

    _myGraphicsScene->update();
    setText(QObject::tr("Delete %1")
                .arg(QString::fromStdString("name=" + gmc->getComponent()->getName() + ", " + position)));
}

DeleteUndoCommand::~DeleteUndoCommand() {
    std::list<ModelComponent*>* allComponents = _myGraphicsScene->getSimulator()->getModels()->current()->getComponents()->getAllComponents();

    if (!allComponents->empty()) {
        const QUndoCommand *lastCommand = _myUndoStack->command(_myUndoStack->count() - 1);
        bool equals = false;

        if (const AddUndoCommand* addUndoCommand = dynamic_cast<const AddUndoCommand*>(lastCommand))
            equals = addUndoCommand->getMyGraphicalModelComponent() && _myGraphicalModelComponent;
        else if (const DeleteUndoCommand* deleteUndoCommand = dynamic_cast<const DeleteUndoCommand*>(lastCommand))
            equals = deleteUndoCommand->getMyGraphicalModelComponent() && _myGraphicalModelComponent;

        if (!equals)
            delete _myGraphicalModelComponent;
    }
}

void DeleteUndoCommand::undo() {
    _myGraphicsScene->addItem(_myGraphicalModelComponent);
    _myGraphicsScene->getGraphicalModelComponents()->append(_myGraphicalModelComponent);

    GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::CREATE, GraphicalModelEvent::EventObjectType::COMPONENT, _myGraphicalModelComponent);
    dynamic_cast<ModelGraphicsView*> (_myGraphicsScene->views().at(0))->notifySceneGraphicalModelEventHandler(modelGraphicsEvent);
    _myGraphicsScene->update();
}

void DeleteUndoCommand::redo() {
    _myGraphicsScene->removeGraphicalModelComponent(_myGraphicalModelComponent, true);
    _myGraphicsScene->update();
}

GraphicalModelComponent* DeleteUndoCommand::getMyGraphicalModelComponent() const {
    return _myGraphicalModelComponent;
}
