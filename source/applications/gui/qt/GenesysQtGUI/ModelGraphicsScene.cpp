/*
 * The MIT License
 *
 * Copyright 2022 rlcancian.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * File:   ModelGraphicsScene.cpp
 * Author: rlcancian
 *
 * Created on 16 de fevereiro de 2022, 09:52
 */

//#include <qt5/QtWidgets/qgraphicssceneevent.h>
#include <QtWidgets/qgraphicssceneevent.h>
#include <QTreeWidget>
#include <QMessageBox>
#include <QUndoCommand>
#include "ModelGraphicsScene.h"
#include "ModelGraphicsView.h"
#include "graphicals/GraphicalModelComponent.h"
#include "graphicals/GraphicalComponentPort.h"
#include "graphicals/GraphicalConnection.h"
#include "actions/AddUndoCommand.h"
#include "actions/DeleteUndoCommand.h"
#include "actions/MoveUndoCommand.h"

ModelGraphicsScene::ModelGraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject *parent) : QGraphicsScene(x, y, width, height, parent) {
	// grid
    _grid.pen.setWidth(TraitsGUI<GScene>::gridPenWidth);
    _grid.pen.setStyle(Qt::DotLine);
}

ModelGraphicsScene::ModelGraphicsScene(const ModelGraphicsScene& orig) { // : QGraphicsScene(orig) {
}

ModelGraphicsScene::~ModelGraphicsScene() {}


//-----------------------------------------------------------------------

GraphicalModelComponent* ModelGraphicsScene::addGraphicalModelComponent(Plugin* plugin, ModelComponent* component, QPointF position, QColor color) {
    // cria o componente gráfico
    GraphicalModelComponent* graphComp = new GraphicalModelComponent(plugin, component, position, color);

    // cria as conexoes
    // verifica se tenho um componente selecionado
	if (selectedItems().size() == 1 && plugin->getPluginInfo()->getMinimumInputs() > 0) { // check if there is selected component and crate a connection between them
        GraphicalModelComponent* otherGraphComp = dynamic_cast<GraphicalModelComponent*> (selectedItems().at(0));

        // verifica se conseguiu converter o item selecionado para GraphicalModelComponent
        if (otherGraphComp != nullptr) { // a component is selected
            // pega o componente selecionado
			ModelComponent* otherComp = otherGraphComp->getComponent();

            // numero maximo de possiveis conexoes pela porta de saida
            unsigned int maxOutputsOtherComp = otherGraphComp->getGraphicalOutputPorts().size();

            // verifica se ainda posso criar conexoes com aquele componente
            if (otherGraphComp->getOcupiedOutputPorts() < maxOutputsOtherComp) {
                // caso tenha portas disponíveis, busca qual delas é
                for (unsigned int numPort = 0; numPort < maxOutputsOtherComp; numPort++) {
                    // caso seja um ponteiro vazio, ele esta livre
                    if (otherComp->getConnections()->getConnectionAtPort(numPort) == nullptr) {
                        // create connection (both model and grapically, since model is being built
                        // model
                        otherGraphComp->getComponent()->getConnections()->insertAtPort(numPort, new Connection({component, 0}));

                        //graphically
                        _sourceGraphicalComponentPort = ((GraphicalModelComponent*) selectedItems().at(0))->getGraphicalOutputPorts().at(numPort);
                        GraphicalComponentPort* destport = graphComp->getGraphicalInputPorts().at(0);
                        addGraphicalConnection(_sourceGraphicalComponentPort, destport, numPort, 0);

                        otherGraphComp->setOcupiedOutputPorts(otherGraphComp->getOcupiedInputPorts() + 1);
                        break;
                    }
                }
            }
        // caso seja uma porta que esteja selecionada
		} else {
			GraphicalComponentPort* sourceGraphPort = dynamic_cast<GraphicalComponentPort*> (selectedItems().at(0));
			if (sourceGraphPort != nullptr) { // a specific output port of a component is selected.
				if (sourceGraphPort->getConnections()->size() == 0) {
					// create connection (both model and grapically, since model is being built (ALMOST REPEATED CODE -- REFACTOR)
					otherGraphComp = sourceGraphPort->graphicalComponent();
					// create connection (both model and grapically, since model is being built (ALMOST REPEATED CODE -- REFACTOR)
					// model
					otherGraphComp->getComponent()->getConnections()->insertAtPort(sourceGraphPort->portNum(), new Connection({component, 0}));
					//graphically
					_sourceGraphicalComponentPort = sourceGraphPort;
					GraphicalComponentPort* destport = graphComp->getGraphicalInputPorts().at(0);
                    addGraphicalConnection(_sourceGraphicalComponentPort, destport, sourceGraphPort->portNum(), 0);
				}
			}
		}
	}

    // adiciona o objeto criado na lista de componentes graficos para nao perder a referencia
    _allGraphicalModelComponents.append(graphComp);

    // cria um objeto para undo e redo do add
    // ele propriamente adiciona o objeto na tela
    QUndoCommand *addUndoCommand = new AddUndoCommand(graphComp, this);
    _undoStack->push(addUndoCommand);
}

GraphicalConnection* ModelGraphicsScene::addGraphicalConnection(GraphicalComponentPort* sourcePort, GraphicalComponentPort* destinationPort, unsigned int portSourceConnection, unsigned int portDestinationConnection) {
    GraphicalConnection* graphicconnection = new GraphicalConnection(sourcePort, destinationPort, portSourceConnection, portDestinationConnection);

    addItem(graphicconnection);

    _graphicalConnections->append(graphicconnection);

    //para limpar referencias das conexoes no final
    _allGraphicalConnections.append(graphicconnection);

    //notify graphical model change
	GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::CREATE, GraphicalModelEvent::EventObjectType::CONNECTION, graphicconnection);
    dynamic_cast<ModelGraphicsView*> (views().at(0))->notifySceneGraphicalModelEventHandler(modelGraphicsEvent);

    return graphicconnection;
}

void ModelGraphicsScene::clearGraphicalModelConnections() {
    // limpa todas as referências das conexões no final
    for (GraphicalConnection* gmc : _allGraphicalConnections) {
        if (gmc) {
            // remove da lista de conexões graficas
            _graphicalConnections->removeOne(gmc);
            _allGraphicalConnections.removeOne(gmc);

            // libera o ponteiro alocado
            delete gmc;
        }
    }
    delete _graphicalConnections;
}

void ModelGraphicsScene::addDrawing() {

}

void ModelGraphicsScene::addAnimation() {

}

void ModelGraphicsScene::removeModelComponentInModel(GraphicalModelComponent* gmc) {
	ModelComponent* component = gmc->getComponent();
	Model* model = _simulator->getModels()->current();
	model->getComponents()->remove(component);
}

void ModelGraphicsScene::removeComponent(GraphicalModelComponent* gmc) {
    // cria um objeto para undo e redo do delete
    // ele propriamente remove o objeto na tela
    QUndoCommand *deleteUndoCommand = new DeleteUndoCommand(gmc, this);
    _undoStack->push(deleteUndoCommand);
}

void ModelGraphicsScene::clearGraphicalModelComponents() {
    QList<GraphicalModelComponent*> *componentsInModel = this->graphicalModelComponentItems();

    for (GraphicalModelComponent* gmc : *componentsInModel) {
        removeModelComponentInModel(gmc);
    }

    // limpa todos os componentes no final, desfazendo as conexoes
    for (GraphicalModelComponent* gmc : _allGraphicalModelComponents) {
        if (gmc) {
            for (GraphicalComponentPort* port : gmc->getGraphicalInputPorts()) {
                for (GraphicalConnection* graphConn : *port->getConnections()) {
                    removeGraphicalConnection(graphConn);
                }
            }

            for (GraphicalComponentPort* port : gmc->getGraphicalOutputPorts()) {
                for (GraphicalConnection* graphConn : *port->getConnections()) {
                    removeGraphicalConnection(graphConn);
                }
            }

            // remove da lista de componentes graficos
            _allGraphicalModelComponents.removeOne(gmc);

            // libera o ponteiro alocado
            delete gmc;
        }
    }

    delete _graphicalModelComponents;
}

// esta funcao trata da remocao das conexoes dos seus "vizinhos" de um componente
void ModelGraphicsScene::handleClearConnectionsOnDeleteComponent(GraphicalModelComponent* gmc) {
    GraphicalModelComponent *destination;
    GraphicalModelComponent *source;

    for (GraphicalComponentPort* port : gmc->getGraphicalInputPorts()) {
        for (GraphicalConnection* graphConn : *port->getConnections()) {
            source = this->findGraphicalModelComponent(graphConn->getSource()->component->getId());

            for (GraphicalComponentPort* portOut : source->getGraphicalOutputPorts()) {
                portOut->removeGraphicalConnection(graphConn);
            }

            source->setOcupiedOutputPorts(source->getOcupiedOutputPorts() - 1);
            graphConn->getSource()->component->getConnections()->removeAtPort(graphConn->getSource()->channel.portNumber);
            removeItem(graphConn);
            _graphicalConnections->removeOne(graphConn);
        }
    }

    for (GraphicalComponentPort* port : gmc->getGraphicalOutputPorts()) {
        for (GraphicalConnection* graphConn : *port->getConnections()) {
            destination = this->findGraphicalModelComponent(graphConn->getDestination()->component->getId());

            for (GraphicalComponentPort* portOut : destination->getGraphicalInputPorts()) {
                portOut->removeGraphicalConnection(graphConn);
            }

            destination->setOcupiedInputPorts(destination->getOcupiedInputPorts() - 1);
            graphConn->getDestination()->component->getConnections()->removeAtPort(graphConn->getDestination()->channel.portNumber);
            removeItem(graphConn);
            _graphicalConnections->removeOne(graphConn);
        }
    }

    //notify graphical model change
    GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::REMOVE, GraphicalModelEvent::EventObjectType::CONNECTION, nullptr); // notify AFTER destroy or BEFORE it?
    dynamic_cast<ModelGraphicsView*> (views().at(0))->notifySceneGraphicalModelEventHandler(modelGraphicsEvent);
}

// esta funcao trata da reconexao de um componente com seus "vizinhos"
void ModelGraphicsScene::reconnectConnectionsOnRedoComponent(GraphicalModelComponent* gmc) {
    GraphicalModelComponent *destination;
    GraphicalModelComponent *source;

    for (GraphicalComponentPort* port : gmc->getGraphicalInputPorts()) {
        for (GraphicalConnection* graphConn : *port->getConnections()) {
            source = this->findGraphicalModelComponent(graphConn->getSource()->component->getId());

            for (GraphicalComponentPort* portOut : source->getGraphicalOutputPorts()) {
                if (portOut->portNum() == graphConn->getSource()->channel.portNumber) {
                    portOut->addGraphicalConnection(graphConn);
                    break;
                }
            }

            source->setOcupiedOutputPorts(source->getOcupiedOutputPorts() + 1);
            graphConn->getSource()->component->getConnections()->insertAtPort(graphConn->getSource()->channel.portNumber, graphConn->getDestination());
            addItem(graphConn);
            _graphicalConnections->append(graphConn);
        }
    }

    for (GraphicalComponentPort* port : gmc->getGraphicalOutputPorts()) {
        for (GraphicalConnection* graphConn : *port->getConnections()) {
            destination = this->findGraphicalModelComponent(graphConn->getDestination()->component->getId());

            for (GraphicalComponentPort* portOut : destination->getGraphicalInputPorts()) {
                if (portOut->portNum() == graphConn->getDestination()->channel.portNumber) {
                    portOut->addGraphicalConnection(graphConn);
                    break;
                }
            }

            destination->setOcupiedInputPorts(destination->getOcupiedInputPorts() + 1);
            addItem(graphConn);
            _graphicalConnections->append(graphConn);
        }
    }

    //notify graphical model change
    GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::REMOVE, GraphicalModelEvent::EventObjectType::CONNECTION, nullptr); // notify AFTER destroy or BEFORE it?
    dynamic_cast<ModelGraphicsView*> (views().at(0))->notifySceneGraphicalModelEventHandler(modelGraphicsEvent);
}

void ModelGraphicsScene::removeConnectionInModel(GraphicalConnection* gc) {
	ModelComponent* sourceComp = gc->getSource()->component;
	sourceComp->getConnections()->removeAtPort(gc->getSource()->channel.portNumber);
}

void ModelGraphicsScene::removeGraphicalConnection(GraphicalConnection* gc) {
	// remove in model
    removeConnectionInModel(gc);

    // remove graphically
	removeItem(gc);
    _graphicalConnections->removeOne(gc);

	//notify graphical model change
	GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::REMOVE, GraphicalModelEvent::EventObjectType::CONNECTION, nullptr); // notify AFTER destroy or BEFORE it?
	dynamic_cast<ModelGraphicsView*> (views().at(0))->notifySceneGraphicalModelEventHandler(modelGraphicsEvent);
}

void ModelGraphicsScene::removeDrawing() {}

void ModelGraphicsScene::removeAnimation() {}


//------------------------------------------------------------------------


void ModelGraphicsScene::showGrid() {
    if (items().size() > 0) {
        for (QGraphicsLineItem* line : *_grid.lines) {
            this->removeItem((QGraphicsItem *)line);
        }
        this->_grid.clear();
        return;
    }

    // clean the grid
    this->_grid.clear();

	// add new grid
	for (int i = sceneRect().left(); i < sceneRect().right(); i += _grid.interval) {
		QGraphicsLineItem* line = addLine(i, sceneRect().top(), i, sceneRect().bottom(), _grid.pen);
        _grid.lines->insert(_grid.lines->end(), line);
	}
	for (int j = sceneRect().top(); j < sceneRect().bottom(); j += _grid.interval) {
		QGraphicsLineItem* line = addLine(sceneRect().left(), j, sceneRect().right(), j, _grid.pen);
        _grid.lines->insert(_grid.lines->end(), line);
	}
}

QUndoStack* ModelGraphicsScene::getUndoStack() {
    return _undoStack;
}

Simulator* ModelGraphicsScene::getSimulator() {
    return _simulator;
}

void ModelGraphicsScene::setUndoStack(QUndoStack* undo) {
    _undoStack = undo;
}

void ModelGraphicsScene::beginConnection() {
	_connectingStep = 1;
	((QGraphicsView*)this->parent())->setCursor(Qt::CrossCursor);
}

//-------------------------
// PROTECTED VIRTUAL FUNCTIONS
//-------------------------

void ModelGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
	QGraphicsScene::mousePressEvent(mouseEvent);
    if (mouseEvent->button() == Qt::LeftButton) {

        QGraphicsItem* item = this->itemAt(mouseEvent->scenePos(), QTransform());

        if (GraphicalModelComponent *component = dynamic_cast<GraphicalModelComponent *> (item)) {
            component->setOldPosition(component->scenePos());
        }

        if (_connectingStep > 0) {
            if (item != nullptr) {
                GraphicalComponentPort* port = dynamic_cast<GraphicalComponentPort*> (item);
                if (port != nullptr) {
                    if (_connectingStep == 1 && !port->isInputPort()) {
                        _sourceGraphicalComponentPort = port;
                        _connectingStep = 2;
                        return;
                    } else if (_connectingStep == 2 && port->isInputPort()) {
                        _connectingStep = 3;
                        // create connection
                        // in the model
                        ModelComponent* sourceComp = _sourceGraphicalComponentPort->graphicalComponent()->getComponent();
                        ModelComponent* destComp = port->graphicalComponent()->getComponent();
                        sourceComp->getConnections()->insertAtPort(_sourceGraphicalComponentPort->portNum(), new Connection({destComp, port->portNum()}));
                        // graphically
                        GraphicalConnection* graphicconnection = new GraphicalConnection(_sourceGraphicalComponentPort, port);
                        _sourceGraphicalComponentPort->addGraphicalConnection(graphicconnection);
                        port->addGraphicalConnection(graphicconnection);
                        addItem(graphicconnection);
                        //
                        ((ModelGraphicsView *) (this->parent()))->unsetCursor();
                        _connectingStep = 0;
                        return;
                    }
                }
            }
            ((ModelGraphicsView *) (this->parent()))->unsetCursor();
            _connectingStep = 0;
        } else {
            if (item==nullptr) {
                //ui->treeViewPropertyEditor->setActiveObject(nullptr, nullptr);
            }
        }
    }
}

void ModelGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    QGraphicsScene::mouseReleaseEvent(mouseEvent);

    QList<GraphicalModelComponent*> components;
    QList<QPointF> oldPositions;
    QList<QPointF> newPositions;

    foreach (QGraphicsItem* item, this->selectedItems()) {
        GraphicalModelComponent* component = dynamic_cast<GraphicalModelComponent*>(item);
        if (component && component->getOldPosition() != component->scenePos()) {
            components.append(component);
            oldPositions.append(component->getOldPosition());
            newPositions.append(component->scenePos());
        }
    }

    if (components.size() >= 1) {
        QUndoCommand *moveUndoCommand = new MoveUndoCommand(components, this, oldPositions, newPositions);
        _undoStack->push(moveUndoCommand);
    }

    foreach (GraphicalModelComponent* item, components) {
        item->setOldPosition(item->scenePos());
    }

}

void ModelGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent) {
	QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
	GraphicalComponentPort* port = dynamic_cast<GraphicalComponentPort*> (this->itemAt(mouseEvent->scenePos(), QTransform()));
    if (port != nullptr) { // if doubleclick on a port, then start connecting
		if (!port->isInputPort() && this->_connectingStep == 0) {
			_sourceGraphicalComponentPort = port;
			_connectingStep = 2;

		}
	}
}

void ModelGraphicsScene::wheelEvent(QGraphicsSceneWheelEvent *wheelEvent) {
    QGraphicsScene::wheelEvent(wheelEvent);
    if (_controlIsPressed)
    {
        QGraphicsView *view = views().isEmpty() ? nullptr : views().first();
        double zoomFactor = 1.1;
        if (wheelEvent->delta() > 0) {
            // Zoom in
            view->scale(zoomFactor, zoomFactor);
        } else {
            // Zoom out
            view->scale(1.0 / zoomFactor, 1.0 / zoomFactor);
        }
        wheelEvent->accept();
    }
}

QList<QGraphicsItem*>*ModelGraphicsScene::getGraphicalEntities() const {
	return _graphicalEntities;
}

QList<QGraphicsItem*>*ModelGraphicsScene::getGraphicalAnimations() const {
	return _graphicalAnimations;
}

QList<QGraphicsItem*>*ModelGraphicsScene::getGraphicalDrawings() const {
	return _graphicalDrawings;
}

QList<QGraphicsItem*>*ModelGraphicsScene::getGraphicalConnections() const {
	return _graphicalConnections;
}

QList<QGraphicsItem*>*ModelGraphicsScene::getGraphicalModelComponents() const {
	return _graphicalModelComponents;
}

void ModelGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
	QGraphicsScene::mouseMoveEvent(mouseEvent);
	((ModelGraphicsView *) (this->parent()))->notifySceneMouseEventHandler(mouseEvent); // to show coords
	if (_connectingStep > 0) {
		QGraphicsItem* item = this->itemAt(mouseEvent->scenePos(), QTransform());
		if (item != nullptr) {
			GraphicalComponentPort* port = dynamic_cast<GraphicalComponentPort*> (item);
			if (port != nullptr) {
				if (_connectingStep == 1 && !port->isInputPort()) {
					((ModelGraphicsView *) (this->parent()))->setCursor(Qt::PointingHandCursor);
				} else if (_connectingStep == 2 && port->isInputPort()) {
					((ModelGraphicsView *) (this->parent()))->setCursor(Qt::PointingHandCursor);
				}
				return;
			}
		}
		if (_connectingStep > 1) {
			((ModelGraphicsView *) (this->parent()))->setCursor(Qt::ClosedHandCursor);
		} else {
			((ModelGraphicsView *) (this->parent()))->setCursor(Qt::CrossCursor);
		}
	}
}

void ModelGraphicsScene::focusInEvent(QFocusEvent *focusEvent) {
	QGraphicsScene::focusInEvent(focusEvent);
}

void ModelGraphicsScene::focusOutEvent(QFocusEvent *focusEvent) {
	QGraphicsScene::focusOutEvent(focusEvent);
}

void ModelGraphicsScene::dropEvent(QGraphicsSceneDragDropEvent *event) {
	QGraphicsScene::dropEvent(event);
	if (this->_objectBeingDragged != nullptr) {
        QTreeWidgetItem*    treeItem = /*dynamic_cast<QTreeWidgetItem*>*/(_objectBeingDragged);
		if (treeItem != nullptr) {			
			QColor color = treeItem->foreground(0).color(); // treeItem->textColor(0);
			QString pluginname = treeItem->whatsThis(0);
			Plugin* plugin = _simulator->getPlugins()->find(pluginname.toStdString());
			if (plugin != nullptr) {
				if (plugin->getPluginInfo()->isComponent()) {
					event->setDropAction(Qt::IgnoreAction);
					event->accept();
					// create component in the model
					ModelComponent* component = (ModelComponent*) plugin->newInstance(_simulator->getModels()->current());
					// create graphically
					addGraphicalModelComponent(plugin, component, event->scenePos(), color);
					return;
				}
			}
		}
	}
	event->setAccepted(false);
}

void ModelGraphicsScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent) {
	QGraphicsScene::contextMenuEvent(contextMenuEvent);
}

void ModelGraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event) {
	QGraphicsScene::dragEnterEvent(event);
	//QString name;
	//Plugin* plugin = _simulator->getPlugins()->find(name.toStdString());
	//if(true) {//(plugin != nullptr) {
	event->setDropAction(Qt::CopyAction);
	event->accept();
	//}
}

void ModelGraphicsScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event) {
	QGraphicsScene::dragLeaveEvent(event);
	event->setDropAction(Qt::CopyAction);
	event->accept();
}

void ModelGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event) {
	QGraphicsScene::dragMoveEvent(event);
	event->setDropAction(Qt::CopyAction);
	event->accept();
}

void ModelGraphicsScene::keyPressEvent(QKeyEvent *keyEvent) {
	QGraphicsScene::keyPressEvent(keyEvent);
	QList<QGraphicsItem*> selected = this->selectedItems();
	if (keyEvent->key() == Qt::Key_Delete && selected.size() > 0) {
		QMessageBox::StandardButton reply = QMessageBox::question(this->_parentWidget, "Delete Component", "Are you sure you want to delete the selected components?", QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::No) {
			return;
        }
		Model* model = _simulator->getModels()->current();
		for (QGraphicsItem* item : selected) {
			GraphicalModelComponent* gmc = dynamic_cast<GraphicalModelComponent*> (item);
			if (gmc != nullptr) {
				// graphically
                removeComponent(gmc);
			} else {
				GraphicalConnection* gc = dynamic_cast<GraphicalConnection*> (item);
				if (gc != nullptr) {
					removeGraphicalConnection(gc);
				} else {
					//Drawing* gc = dynamic_cast<GraphicalConnection*>(item);
					//if (gc != nullptr) {
					//	removeItem(item);
					//	gc->~Drawing();
					//}
				}
			}
		}
	}
	_controlIsPressed = (keyEvent->key() == Qt::Key_Control);
}

void ModelGraphicsScene::keyReleaseEvent(QKeyEvent *keyEvent) {
	QGraphicsScene::keyReleaseEvent(keyEvent);
	if (_controlIsPressed)
		_controlIsPressed = (keyEvent->key() != Qt::Key_Control);
}

//--------------------------
//
//--------------------------

void ModelGraphicsScene::setObjectBeingDragged(QTreeWidgetItem* objectBeingDragged) {
	_objectBeingDragged = objectBeingDragged;
}

void ModelGraphicsScene::setSimulator(Simulator *simulator) {
	_simulator = simulator;
}

unsigned short ModelGraphicsScene::connectingStep() const {
	return _connectingStep;
}

void ModelGraphicsScene::setConnectingStep(unsigned short connectingStep) {
	_connectingStep = connectingStep;
}

void ModelGraphicsScene::setParentWidget(QWidget *parentWidget) {
	_parentWidget = parentWidget;
}


QList<GraphicalModelComponent*>* ModelGraphicsScene::graphicalModelComponentItems(){
	QList<GraphicalModelComponent*>* list = new QList<GraphicalModelComponent*>();
	for(QGraphicsItem* item: this->items()) {
		GraphicalModelComponent* gmc = dynamic_cast<GraphicalModelComponent*>(item);
		if (gmc != nullptr) {
			list->append(gmc);
		}
	}
	return list;
}

GraphicalModelComponent* ModelGraphicsScene::findGraphicalModelComponent(Util::identification id){
    QList<GraphicalModelComponent*> *allComponents = ModelGraphicsScene::graphicalModelComponentItems();

    for(GraphicalModelComponent* item: *allComponents) {
        if (item->getComponent()->getId() == id) {
            return item;
        }
    }
    return nullptr;
}

//------------------------
// Private
//------------------------

