#include "AddUndoCommand.h"
#include "ModelGraphicsScene.h"

AddUndoCommand::AddUndoCommand(GraphicalModelComponent *gmc, ModelGraphicsScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent), myGraphicsScene(scene) {
    static int itemCount = 0;
    first = true;

//    ModelComponent *component = gmc->getComponent();
//    std::string pluginName = component->getClassname();
//    Plugin *plugin = scene->getSimulator()->getPlugins()->find(pluginName);
//    initialPosition = QPointF(gmc->scenePos().x(), gmc->scenePos().y());
//    ModelComponent *newComponent = (ModelComponent*) plugin->newInstance(scene->getSimulator()->getModels()->current());

//    myGraphicalModelComponent = new GraphicalModelComponent(plugin, newComponent, initialPosition, Qt::blue);
    myGraphicalModelComponent = gmc;
    std::string position = "position=(x=" + std::to_string(gmc->scenePos().x()) + ", y=" + std::to_string(gmc->scenePos().y()) + ")";
    myGraphicsScene->update();
    ++itemCount;
    setText(QObject::tr("Add %1")
                .arg(QString::fromStdString("name=" + gmc->getComponent()->getName() + ", " + position)));
}

AddUndoCommand::~AddUndoCommand() {}

void AddUndoCommand::undo() {
    myGraphicsScene->removeGraphicalModelComponent(myGraphicalModelComponent);
    myGraphicsScene->update();
}

void AddUndoCommand::redo() {
    if (!first) {
        ModelGraphicsScene *cena = myGraphicsScene;
        GraphicalModelComponent *comp = myGraphicalModelComponent;
        myGraphicsScene->addItemToScene(myGraphicalModelComponent);
        myGraphicsScene->getGraphicalModelComponents()->append(myGraphicalModelComponent);

        GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::CREATE, GraphicalModelEvent::EventObjectType::COMPONENT, myGraphicalModelComponent);
        dynamic_cast<ModelGraphicsView*> (myGraphicsScene->views().at(0))->notifySceneGraphicalModelEventHandler(modelGraphicsEvent);

        myGraphicalModelComponent->setPos(initialPosition);
        myGraphicsScene->clearSelection();
        myGraphicsScene->update();
    }
    first = false;
}
