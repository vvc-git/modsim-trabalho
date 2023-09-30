#ifndef GRAPHICALMODELDATADEFINITION_H
#define GRAPHICALMODELDATADEFINITION_H

/*
 * File:   ModelDataDefinitionGraphicItem.h
 * Author: rlcancian
 *
 * Created on 16 de fevereiro de 2022, 11:41
 */

#include <QGraphicsItem>
#include <QGraphicsObject>
#include <QPen>
#include <QBrush>
#include "../../../../kernel/simulator/Plugin.h"
#include "TraitsGUI.h"

class GraphicalModelDataDefinition : public QGraphicsObject {
public:
	GraphicalModelDataDefinition(Plugin* plugin, ModelDataDefinition* element, QPointF position, QColor color = Qt::blue, QGraphicsItem *parent = nullptr);
	GraphicalModelDataDefinition(const GraphicalModelDataDefinition& orig);
	virtual ~GraphicalModelDataDefinition();
public:
	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
	ModelDataDefinition* getDataDefinition() const;
protected:
	QColor myrgba(uint64_t color); // TODO: Should NOT be here, but in UtilGUI.h, but then it generates multiple definitions error
protected: // virtual
	virtual bool sceneEvent(QEvent *event) override;
	//virtual void	hoverEnterEvent(QGraphicsSceneHoverEvent * event)
	//virtual void	hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
	//virtual void	hoverMoveEvent(QGraphicsSceneHoverEvent * event)
	//virtual void	inputMethodEvent(QInputMethodEvent * event)
	//virtual QVariant	inputMethodQuery(Qt::InputMethodQuery query) const
	//virtual QVariant	itemChange(GraphicsItemChange change, const QVariant & value)
	//virtual void	keyPressEvent(QKeyEvent * event)
	//virtual void	keyReleaseEvent(QKeyEvent * event)
	//virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
	//virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
	//virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
	//virtual void	mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
protected:
	qreal _width = TraitsGUI<GModelDataDefinition>::width; //150;
	qreal _height = _width * TraitsGUI<GModelDataDefinition>::heightProportion; //0.67;
	unsigned int _margin = TraitsGUI<GModelDataDefinition>::margin;//8;
	unsigned int _selWidth = TraitsGUI<GModelDataDefinition>::selectionWidth;//8;
	ModelDataDefinition* _element;
	QColor _color;
	qreal _stretchPosTop = TraitsGUI<GModelDataDefinition>::stretchPos;//0.5;
	qreal _stretchPosBottom = TraitsGUI<GModelDataDefinition>::stretchPos;//0.5;
	qreal _stretchPosLeft = TraitsGUI<GModelDataDefinition>::stretchPos;//0.5;
	qreal _stretchPosRigth = TraitsGUI<GModelDataDefinition>::stretchPos;//0.5;
	qreal _stretchRigth = TraitsGUI<GModelDataDefinition>::stretch;//0;
	qreal _stretchLeft = TraitsGUI<GModelDataDefinition>::stretch;//0;
	qreal _stretchRigthMidle = TraitsGUI<GModelDataDefinition>::stretch;//0;
	qreal _stretchLeftMidle = TraitsGUI<GModelDataDefinition>::stretch;//0;
	qreal _stretchTop = TraitsGUI<GModelDataDefinition>::stretch;//0;
	qreal _stretchBottom = TraitsGUI<GModelDataDefinition>::stretch;//0;
	qreal _stretchTopMidle = TraitsGUI<GModelDataDefinition>::stretch;//0;
	qreal _stretchBottomMidle = TraitsGUI<GModelDataDefinition>::stretch;//0;
private:
};

#endif /* GRAPHICALMODELDATADEFINITION_H */

