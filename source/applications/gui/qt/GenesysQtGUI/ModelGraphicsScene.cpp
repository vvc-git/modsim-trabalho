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
#include "ModelGraphicsScene.h"
#include "ModelGraphicsView.h"
#include "graphicals/GraphicalModelComponent.h"
#include "graphicals/GraphicalComponentPort.h"
#include "graphicals/GraphicalConnection.h"

ModelGraphicsScene::ModelGraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject *parent) : QGraphicsScene(x, y, width, height, parent) {
    // grid
    _grid.interval = TraitsGUI<GScene>::gridInterval; // 20;
    _grid.pen = QPen(TraitsGUI<GScene>::gridColor);	  // QPen(Qt::gray); //TODO: To use TraitsGUI<GScene>::gridColor must solve myrgba first
    _grid.lines = new std::list<QGraphicsLineItem *>();
    _grid.visible = false;

	_grid.pen.setWidth(TraitsGUI<GScene>::gridPenWidth);
	_grid.pen.setStyle(Qt::DotLine);
}

ModelGraphicsScene::ModelGraphicsScene(const ModelGraphicsScene& orig) { // : QGraphicsScene(orig) {
}

ModelGraphicsScene::~ModelGraphicsScene() {
}


//-----------------------------------------------------------------------

GraphicalModelComponent* ModelGraphicsScene::addGraphicalModelComponent(Plugin* plugin, ModelComponent* component, QPointF position, QColor color) {
	GraphicalModelComponent* graphComp = new GraphicalModelComponent(plugin, component, position, color);
	addItem(graphComp);
	_graphicalModelComponents->append(graphComp);
	if (selectedItems().size() == 1 && plugin->getPluginInfo()->getMinimumInputs() > 0) { // check if there is selected component and crate a connection between them
		GraphicalModelComponent* otherGraphComp = dynamic_cast<GraphicalModelComponent*> (selectedItems().at(0));
		if (otherGraphComp != nullptr) { // a component is selected
			ModelComponent* otherComp = otherGraphComp->getComponent();
			unsigned int i = 0;
			bool connCreated = false;
			while (i < otherComp->getConnections()->getMaxOutputConnections() && !connCreated) {
				if (otherComp->getConnections()->getConnectionAtPort(i) == nullptr) {
					// create connection (both model and grapically, since model is being built
					// model
					otherGraphComp->getComponent()->getConnections()->insertAtPort(i, new Connection({component, 0}));
					//graphically
					_sourceGraphicalComponentPort = ((GraphicalModelComponent*) selectedItems().at(0))->getGraphicalOutputPorts().at(i);
					GraphicalComponentPort* destport = graphComp->getGraphicalInputPorts().at(0);
					addGraphicalConnection(_sourceGraphicalComponentPort, destport);
					connCreated = true;
				}
				i++;
			}
			if (!connCreated && otherComp->getConnections()->size() < plugin->getPluginInfo()->getMaximumOutputs()) {
				// create connection (both model and grapically, since model is being built (ALMOST REPEATED CODE -- REFACTOR)
				// model
				i = otherComp->getConnections()->size();
				otherGraphComp->getComponent()->getConnections()->insertAtPort(i, new Connection({component, 0}));
				//graphically
				_sourceGraphicalComponentPort = ((GraphicalModelComponent*) selectedItems().at(0))->getGraphicalOutputPorts().at(i);
				GraphicalComponentPort* destport = graphComp->getGraphicalInputPorts().at(0);
				addGraphicalConnection(_sourceGraphicalComponentPort, destport);
			}
		} else {
			GraphicalComponentPort* sourceGraphPort = dynamic_cast<GraphicalComponentPort*> (selectedItems().at(0));
			if (sourceGraphPort != nullptr) { // a specific output port of a component is selected.
				if (sourceGraphPort->getConnections()->size() == 0) {
					// create connection (both model and grapically, since model is being built (ALMOST REPEATED CODE -- REFACTOR)
					otherGraphComp = sourceGraphPort->graphicalComponent();
					ModelComponent* otherComp = otherGraphComp->getComponent();
					// create connection (both model and grapically, since model is being built (ALMOST REPEATED CODE -- REFACTOR)
					// model
					otherGraphComp->getComponent()->getConnections()->insertAtPort(sourceGraphPort->portNum(), new Connection({component, 0}));
					//graphically
					_sourceGraphicalComponentPort = sourceGraphPort;
					GraphicalComponentPort* destport = graphComp->getGraphicalInputPorts().at(0);
					addGraphicalConnection(_sourceGraphicalComponentPort, destport);
				}
			}
		}
	}
	//notify graphical model change
	GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::CREATE, GraphicalModelEvent::EventObjectType::COMPONENT, graphComp);
	dynamic_cast<ModelGraphicsView*> (views().at(0))->notifySceneGraphicalModelEventHandler(modelGraphicsEvent);
	return graphComp;
}

GraphicalConnection* ModelGraphicsScene::addGraphicalConnection(GraphicalComponentPort* sourcePort, GraphicalComponentPort* destinationPort) {
	GraphicalConnection* graphicconnection = new GraphicalConnection(sourcePort, destinationPort);
	addItem(graphicconnection);
	_graphicalConnections->append(graphicconnection);
	//notify graphical model change
	GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::CREATE, GraphicalModelEvent::EventObjectType::CONNECTION, graphicconnection);
	dynamic_cast<ModelGraphicsView*> (views().at(0))->notifySceneGraphicalModelEventHandler(modelGraphicsEvent);
	return graphicconnection;
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

void ModelGraphicsScene::removeGraphicalModelComponent(GraphicalModelComponent* gmc) {
	// remove graphically
	/// first remove connections
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
	/// then remove the component
	// remove in model
	removeModelComponentInModel(gmc);
	//graphically
	removeItem(gmc);
	_graphicalModelComponents->removeOne(gmc);
	ModelComponent* component = gmc->getComponent();
	gmc->~GraphicalModelComponent();
	//notify graphical model change
	GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::REMOVE, GraphicalModelEvent::EventObjectType::COMPONENT, nullptr); // notify AFTER destroy or BEFORE it?
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
	// //////gc->~GraphicalConnection();
	//notify graphical model change
	GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::REMOVE, GraphicalModelEvent::EventObjectType::CONNECTION, nullptr); // notify AFTER destroy or BEFORE it?
	dynamic_cast<ModelGraphicsView*> (views().at(0))->notifySceneGraphicalModelEventHandler(modelGraphicsEvent);
}

void ModelGraphicsScene::removeDrawing() {

}

void ModelGraphicsScene::removeAnimation() {

}


//------------------------------------------------------------------------

// retorna o elemento _grid que é privado
ModelGraphicsScene::GRID *ModelGraphicsScene::grid() {
    return &_grid;
}

// implementação da função clear() da estrutura GRID
void ModelGraphicsScene::GRID::clear() {
    // limpa e libera a memória da lista de linhas
    for (QGraphicsLineItem *line : *lines) {
        delete line;
    }
    lines->clear();

    // volta a visibilidade pra false
    visible = false;
}

void ModelGraphicsScene::showGrid()
{
    // pego a informação se o grid está visível
    // obs.: o grid é criado uma única vez para a cena e habilitado como visível ou não. =

    // se eu quero que o grid fique visível, verifico se o grid já está desenhado ou não
    if (_grid.visible) {
        // se não tenho linhas no grid, eu as desenho
        if (_grid.lines->size() <= 0) {
            // add new grid
            for (int i = sceneRect().left(); i < sceneRect().right(); i += _grid.interval) {
                QGraphicsLineItem *line = addLine(i, sceneRect().top(), i, sceneRect().bottom(), _grid.pen);
                line->setZValue(-1.0);
                line->setVisible(true);
                _grid.lines->insert(_grid.lines->end(), line);
            }
            for (int j = sceneRect().top(); j < sceneRect().bottom(); j += _grid.interval) {
                QGraphicsLineItem *line = addLine(sceneRect().left(), j, sceneRect().right(), j, _grid.pen);
                line->setZValue(-1.0);
                line->setVisible(true);
                _grid.lines->insert(_grid.lines->end(), line);
            }
        }
        // se eu já tenho meu grid desenhado eu apenas o torno visível
        else {
            for (QGraphicsLineItem *line : *_grid.lines) {
                line->setVisible(true);
            }
        }
    }
    // se eu quero esconder o grid eu tiro a visibilidade das linhas
    else {
        for (QGraphicsLineItem *line : *_grid.lines) {
            line->setVisible(false);
        }
    }

    // troco o valor de visible
    _grid.visible = !_grid.visible;
}

void ModelGraphicsScene::beginConnection() {
	_connectingStep = 1;
	((QGraphicsView*)this->parent())->setCursor(Qt::CrossCursor);
}

void ModelGraphicsScene::groupComponents() {
    int size = selectedItems().size();
    int num_groups = getGraphicalGroups()->size();
    //verifica se algum item selecionado já faz parte de um grupo
    bool component_in_group = false;
    if (size > 1 && num_groups > 0) {
        for (int i = 0; (i < size) && !component_in_group; i++) {  //percorrer todos os itens selecionados
            QGraphicsItem* c = selectedItems().at(i);
            int group_children = c->childItems().size();
            if (group_children > 1) {
                component_in_group = true;
            }
        }
    }
    if (!component_in_group) {

        QList<QGraphicsItem*> group = selectedItems();

        QGraphicsItemGroup* new_group = new QGraphicsItemGroup();

        for (int i = 0; i < group.size(); i++) {
            QGraphicsItem* c = group.at(i);
            new_group->addToGroup(c);
        }

        // Adicione o novo grupo à sua cena
        addItem(new_group);
        new_group->setFlag(QGraphicsItem::ItemIsSelectable, true);
        new_group->setFlag(QGraphicsItem::ItemIsMovable, true);
        // Adicione o grupo à sua lista de grupos (se necessário)
        getGraphicalGroups()->append(new_group);
    }
}

void ModelGraphicsScene::ungroupComponents() {
    int size = selectedItems().size();
    if (size == 1) {
        QGraphicsItem* item = selectedItems().at(0);
        QGraphicsItemGroup* group = dynamic_cast<QGraphicsItemGroup*>(item);

        if (group) {
            // Recupere os itens individuais no grupo
            QList<QGraphicsItem*> itemsInGroup = group->childItems();


            // Adicione novamente os itens individuais à cena
            for (int i = 0; i < itemsInGroup.size(); i++) {
                QGraphicsItem * item = itemsInGroup.at(i);
                //remova item por item do grupo
                group->removeFromGroup(item);
                //adicionar novamente a cena
                addItem(item);
                item->setFlag(QGraphicsItem::ItemIsSelectable, true);
                item->setFlag(QGraphicsItem::ItemIsMovable, true);
            }
            // Remova o grupo da cena
            removeItem(group);
            delete group;

        }
    }
}


void ModelGraphicsScene::arranjeModels(int direction) {
    int size = selectedItems().size();
    qreal most_direction;
    qreal most_up;
    qreal most_down;
    qreal most_left;
    qreal most_right;
    qreal middle;
    qreal center;

    if (size >= 2) {
        switch (direction) {
        case 0: //left
            most_direction = sceneRect().right();
            break;
        case 1: //right
            most_direction = sceneRect().left();
            break;
        case 2: //top
            most_direction = sceneRect().bottom();
            break;
        case 3: //bottom
            most_direction = sceneRect().top();
            break;
        case 4: //center
            most_left = sceneRect().right();
            most_right = sceneRect().left();
            for (int i =0; i < size; i++) {
                GraphicalModelComponent* c = dynamic_cast<GraphicalModelComponent*> (selectedItems().at(i));
                qreal item_posX = c->x();
                if (item_posX < most_left) {
                    most_left = item_posX;
                }
                if (item_posX > most_right) {
                    most_right = item_posX;
                }
            }
            center = (most_right + most_left) / 2;
            for (int i =0; i < size; i++) {
                GraphicalModelComponent* c = dynamic_cast<GraphicalModelComponent*> (selectedItems().at(i));
                c->setX(center);
            }
            break;
        case 5: //middle
            most_up = sceneRect().bottom();
            most_down = sceneRect().top();
            for (int i =0; i < size; i++) {
                GraphicalModelComponent* c = dynamic_cast<GraphicalModelComponent*> (selectedItems().at(i));
                qreal item_posY = c->y();
                if (item_posY < most_up) {
                    most_up = item_posY;
                }
                if (item_posY > most_down) {
                    most_down = item_posY;
                }
            }
            middle = (most_up + most_down) / 2;
            for (int i =0; i < size; i++) {
                GraphicalModelComponent* c = dynamic_cast<GraphicalModelComponent*> (selectedItems().at(i));
                c->setY(middle);
            }
            break;
        }
        if (direction < 4) {
            for (int i =0; i < size; i++) {
                GraphicalModelComponent* c = dynamic_cast<GraphicalModelComponent*> (selectedItems().at(i));
                if (direction < 2) {
                    qreal item_pos = c->x();
                    if ((item_pos < most_direction && direction == 0) || (item_pos > most_direction && direction == 1) ) {
                        most_direction = item_pos;
                    }
                } else {
                    qreal item_pos = c->y();
                    if ((item_pos < most_direction && direction == 2) || (item_pos > most_direction && direction == 3) ) {
                        most_direction = item_pos;
                    }
                }
            }
            if (direction < 2) {
                for (int i =0; i < size; i++) {
                    GraphicalModelComponent* c = dynamic_cast<GraphicalModelComponent*> (selectedItems().at(i));
                    c->setX(most_direction);
                }
            } else {
                for (int i =0; i < size; i++) {
                    GraphicalModelComponent* c = dynamic_cast<GraphicalModelComponent*> (selectedItems().at(i));
                    c->setY(most_direction);
                }
            }
        }
    }
}


//-------------------------
// PROTECTED VIRTUAL FUNCTIONS
//-------------------------

void ModelGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
	QGraphicsScene::mousePressEvent(mouseEvent);
    if (mouseEvent->button() == Qt::LeftButton) {
        QGraphicsItem* item = this->itemAt(mouseEvent->scenePos(), QTransform());
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
	if (_controlIsPressed) {
		//@TODO: ZOOM!!
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

QList<QGraphicsItemGroup*>*ModelGraphicsScene::getGraphicalGroups() const {
    return _graphicalGroups;
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
		QTreeWidgetItem* treeItem = /*dynamic_cast<QTreeWidgetItem*>*/(_objectBeingDragged);
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
				// remove in model
				removeModelComponentInModel(gmc);
				// graphically
				removeGraphicalModelComponent(gmc);
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

/*
QList<GraphicalModelComponent*>* ModelGraphicsScene::graphicalModelMomponentItems(){
	QList<GraphicalModelComponent*>* list = new QList<GraphicalModelComponent*>();
	for(QGraphicsItem* item: this->items()) {
		GraphicalModelComponent* gmc = dynamic_cast<GraphicalModelComponent*>(item);
		if (gmc != nullptr) {
			list->append(gmc);
		}
	}
	return list;
}
 */

//------------------------
// Private
//------------------------

