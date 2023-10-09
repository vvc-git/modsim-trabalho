#include "AddUndoCommand.h"
#include "ModelGraphicsScene.h"

AddUndoCommand::AddUndoCommand(GraphicalModelComponent *gmc, ModelGraphicsScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent), myGraphicsScene(scene) {
    static int itemCount = 0;
    first = true;
    initialPosition = QPointF(gmc->scenePos().x(), gmc->scenePos().y() + gmc->getHeight()/2);
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
        //myGraphicsScene->update();
        ModelComponent *component = myGraphicalModelComponent->getComponent();
        std::string pluginName = component->getClassname();
        Plugin *plugin = myGraphicsScene->getSimulator()->getPlugins()->find(pluginName);
        QColor color = myGraphicalModelComponent->getColor();
        myGraphicsScene->clearSelection();

        myGraphicsScene->getSimulator()->getModels()->current()->insert(component);
        myGraphicalModelComponent = myGraphicsScene->addGraphicalModelComponent(plugin, component, initialPosition, color, true);

        myGraphicsScene->update();
    }
    first = false;
}
