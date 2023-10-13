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

void ModelGraphicsScene::addDrawing(QPointF endPoint, bool moving) {
    if (_drawingMode == LINE) {
        //verifica se a linha é muito pequena antes de desenhar
        if (abs(_drawingStartPoint.x() - endPoint.x()) > _grid.interval || abs(_drawingStartPoint.y() - endPoint.y()) > _grid.interval) {

            if (_currentLine != nullptr) {
                removeItem(_currentLine);
                delete _currentLine;
            }

            // Crie e adicione um nova linha à cena
            if (moving) {
                _currentLine = new QGraphicsLineItem(_drawingStartPoint.x(), _drawingStartPoint.y(), endPoint.x(), endPoint.y());
                addItem(_currentLine);
            } else {
                QGraphicsLineItem* line = new QGraphicsLineItem(_drawingStartPoint.x(), _drawingStartPoint.y(), endPoint.x(), endPoint.y());
                line->setFlag(QGraphicsItem::ItemIsSelectable, true);
                line->setFlag(QGraphicsItem::ItemIsMovable, true);
                getGraphicalDrawings()->append(line);
                addItem(line);
            }
        }

    } else if (_drawingMode == TEXT) {
        /*
        QGraphicsTextItem* textItem = new QGraphicsTextItem("Seu Texto Aqui");
        textItem->setFont(QFont("Arial", 12)); // Configurar a fonte (opcional)
        textItem->setPos(endPoint.x(), endPoint.y()); // Definir a posição na cena
        textItem->setDefaultTextColor(Qt::black); // Configurar a cor do texto
        textItem->setTextWidth(30*_grid.interval); // Definir largura máxima para quebrar o texto
        textItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
        textItem->setFlag(QGraphicsItem::ItemIsMovable, true);
        textItem->setVisible(true); */

        //connect(this, &QGraphicsScene::selectionChanged, this, &ModelGraphicsScene::startTextEditing);

    } else if (_drawingMode == RECTANGLE) {
        if (abs(_drawingStartPoint.x() - endPoint.x()) > _grid.interval && abs(_drawingStartPoint.y() - endPoint.y()) > _grid.interval) {
            // Remova o retângulo anterior, se houver
            qreal width = endPoint.x() - _drawingStartPoint.x();
            qreal height = endPoint.y() - _drawingStartPoint.y();
            if (_currentRectangle != nullptr) {
                removeItem(_currentRectangle);
                delete _currentRectangle;
            }

            // Crie e adicione um novo retângulo à cena
            if (moving) {
                _currentRectangle = new QGraphicsRectItem(_drawingStartPoint.x(), _drawingStartPoint.y(), width, height);
                addItem(_currentRectangle);
            } else {
                QGraphicsRectItem* rectangle = new QGraphicsRectItem(_drawingStartPoint.x(), _drawingStartPoint.y(), width, height);
                rectangle->setFlag(QGraphicsItem::ItemIsSelectable, true);
                rectangle->setFlag(QGraphicsItem::ItemIsMovable, true);
                getGraphicalDrawings()->append(rectangle);
                addItem(rectangle);
            }
        }
    } else if (_drawingMode == ELLIPSE) {
        if (abs(_drawingStartPoint.x() - endPoint.x()) > _grid.interval && abs(_drawingStartPoint.y() - endPoint.y()) > _grid.interval) {
            // Remova a ellipse anterior, se houver
            qreal width = endPoint.x() - _drawingStartPoint.x();
            qreal height = endPoint.y() - _drawingStartPoint.y();
            if (_currentEllipse != nullptr) {
                removeItem(_currentEllipse);
                delete _currentEllipse;
            }

            // Crie e adicione uma nova ellipse à cena
            if (moving) {
                _currentEllipse = new QGraphicsEllipseItem(_drawingStartPoint.x(), _drawingStartPoint.y(), width, height);
                addItem(_currentEllipse);
            } else {
                QGraphicsEllipseItem* ellipse = new QGraphicsEllipseItem(_drawingStartPoint.x(), _drawingStartPoint.y(), width, height);
                ellipse->setFlag(QGraphicsItem::ItemIsSelectable, true);
                ellipse->setFlag(QGraphicsItem::ItemIsMovable, true);
                getGraphicalDrawings()->append(ellipse);
                addItem(ellipse);
            }
        }
    } else if (_drawingMode == POLYGON) {
        // Adiciona o primeiro ponto do poligono
        _drawingMode = POLYGON_POINTS;
        _currentPolygonPoints.clear();
        _currentPolygonPoints << endPoint;
        _currentPolygon = new QGraphicsPolygonItem(QPolygonF(_currentPolygonPoints));
        _currentPolygon->setVisible(true);

        addItem(_currentPolygon);
    } else if (_drawingMode == POLYGON_POINTS) {
        removeItem(_currentPolygon);
        _currentPolygonPoints << endPoint;
        _currentPolygon = new QGraphicsPolygonItem(QPolygonF(_currentPolygonPoints));
        _currentPolygon->setVisible(true);
        addItem(_currentPolygon);
    } else if (_drawingMode == POLYGON_FINISHED) {
        _currentPolygon->setFlag(QGraphicsItem::ItemIsSelectable, true);
        _currentPolygon->setFlag(QGraphicsItem::ItemIsMovable, true);
        getGraphicalDrawings()->append(_currentPolygon);
    }
    // Redefina o estado de desenho
    if (!moving && !(_drawingMode == POLYGON) && !(_drawingMode == POLYGON_POINTS)) {
        _drawingMode = NONE;
        _currentRectangle = nullptr;
        _currentLine = nullptr;
        _currentEllipse = nullptr;
        _currentPolygon = nullptr;
    }
}

void ModelGraphicsScene::addAnimation() {

}

void ModelGraphicsScene::startTextEditing() {
    QGraphicsItem* selectedItem = focusItem();
    if (selectedItem) {
        QGraphicsTextItem* textItem = dynamic_cast<QGraphicsTextItem*>(selectedItem);
        if (textItem) {
            textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
            textItem->setFocus(); // Dê foco ao item para iniciar a edição de texto
        }
    }
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

void ModelGraphicsScene::removeDrawing(QGraphicsItem * item) {
    for (int i = 0 ; i < getGraphicalDrawings()->size(); i++) {
        if (getGraphicalDrawings()->at(i) == item) {
            getGraphicalDrawings()->removeAt(i);
            //removeItem(item);
            delete(item);
        }
    }
}

void ModelGraphicsScene::removeAnimation() {

}


//------------------------------------------------------------------------

void ModelGraphicsScene::showGrid() {
	// remove existing grid
	if (items().size() > 0) {
	//	for (QGraphicsLineItem* line : *_grid.lines) {
	//		removeItem((QGraphicsItem*) line);
	//	}
	}
	_grid.lines->clear();
	// add new grid
	for (int i = sceneRect().left(); i < sceneRect().right(); i += _grid.interval) {
		QGraphicsLineItem* line = addLine(i, sceneRect().top(), i, sceneRect().bottom(), _grid.pen);
	//	_grid.lines->insert(_grid.lines->end(), line);
	}
	for (int j = sceneRect().top(); j < sceneRect().bottom(); j += _grid.interval) {
		QGraphicsLineItem* line = addLine(sceneRect().left(), j, sceneRect().right(), j, _grid.pen);
	//	_grid.lines->insert(_grid.lines->end(), line);
	}
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
        } else if (_drawingMode != NONE) {
            // Capturar o ponto de início do desenho
            _drawingStartPoint = mouseEvent->scenePos();
            _currentRectangle = nullptr;
            _currentLine = nullptr;
            _currentEllipse = nullptr;

            if (_drawingMode == POLYGON) {
                // Cria o poligono
                addDrawing(_drawingStartPoint, false);
            } else if (_drawingMode == POLYGON_POINTS) {
                // Continue a adicionar pontos ao polígono
                addDrawing(mouseEvent->scenePos(), false);
            } else if (_drawingMode == TEXT) {
                addDrawing(mouseEvent->scenePos(), false);
            }
        }
    }
}

void ModelGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {
	QGraphicsScene::mouseReleaseEvent(mouseEvent);

    if (mouseEvent->button() == Qt::LeftButton && _drawingMode != NONE) {
        // Capturar o ponto final da linha
        QPointF drawingEndPoint = mouseEvent->scenePos();
        //Adicionar desenho a tela
        addDrawing(drawingEndPoint, false);
        ((ModelGraphicsView *) (this->parent()))->unsetCursor();
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
    if (_drawingMode == POLYGON || _drawingMode == POLYGON_POINTS) {
        _drawingMode = POLYGON_FINISHED;
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
    } else if (_drawingMode != NONE){
        //mostrar desenho se formando
        QPointF currentPoint = mouseEvent->scenePos();
        addDrawing(currentPoint, true);
        ((ModelGraphicsView *) (this->parent()))->setCursor(Qt::SplitHCursor);
        /*if (_drawingMode == LINE) {
            QPointF currentPoint = mouseEvent->scenePos();
            addDrawing(currentPoint, true);
            ((ModelGraphicsView *) (this->parent()))->setCursor(Qt::SplitHCursor);
        } else if (_drawingMode == RECTANGLE) {
            QPointF currentPoint = mouseEvent->scenePos();
            addDrawing(currentPoint, true);
            ((ModelGraphicsView *) (this->parent()))->setCursor(Qt::SplitHCursor);
        } else if (_drawingMode == ELLIPSE) {
            QPointF currentPoint = mouseEvent->scenePos();
            addDrawing(currentPoint, true);
            ((ModelGraphicsView *) (this->parent()))->setCursor(Qt::ClosedHandCursor);
        }*/
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
                    bool aux = getGraphicalDrawings()->contains(item);
                    if (aux) {
                        removeDrawing(item);
                    }
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

void ModelGraphicsScene::setDrawingMode(DrawingMode drawingMode) {
    _drawingMode = drawingMode;
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

