#include "AddUndoCommand.h"
#include "DeleteUndoCommand.h"
#include "ModelGraphicsView.h"

AddUndoCommand::AddUndoCommand(GraphicalModelComponent *gmc, ModelGraphicsScene *scene, QUndoStack* stack, QUndoCommand *parent)
    : QUndoCommand(parent), _myGraphicalModelComponent(gmc), _myGraphicsScene(scene), _myUndoStack(stack) {
    _firstExecution = true;
    _initialPosition = QPointF(gmc->scenePos().x(), gmc->scenePos().y() + gmc->getHeight()/2);

    std::string position = "position=(x=" + std::to_string(gmc->scenePos().x()) + ", y=" + std::to_string(gmc->scenePos().y()) + ")";

    _myGraphicsScene->update();
    setText(QObject::tr("Add %1")
                .arg(QString::fromStdString("name=" + gmc->getComponent()->getName() + ", " + position)));
}

AddUndoCommand::~AddUndoCommand() {
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

void AddUndoCommand::undo() {
    _myGraphicsScene->removeGraphicalModelComponent(_myGraphicalModelComponent, true);
    _myGraphicsScene->update();
}

void AddUndoCommand::redo() {
    if (!_firstExecution) {
        _myGraphicsScene->addItem(_myGraphicalModelComponent);
        _myGraphicsScene->getGraphicalModelComponents()->append(_myGraphicalModelComponent);

        GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::CREATE, GraphicalModelEvent::EventObjectType::COMPONENT, _myGraphicalModelComponent);
        dynamic_cast<ModelGraphicsView*> (_myGraphicsScene->views().at(0))->notifySceneGraphicalModelEventHandler(modelGraphicsEvent);
        _myGraphicsScene->update();
    }
    _firstExecution = false;
}

GraphicalModelComponent* AddUndoCommand::getMyGraphicalModelComponent() const {
    return _myGraphicalModelComponent;
}
