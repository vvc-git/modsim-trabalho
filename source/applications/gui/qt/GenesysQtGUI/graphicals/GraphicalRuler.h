#ifndef GRAPHICALRULER_H
#define GRAPHICALRULER_H

#include <QWidget>
#include <QAbstractScrollArea>
#include <QApplication>
#include <QResizeEvent>
#include <QScrollBar>
#include <QPainter>
#include <QString>
#include <QScreen>

class GraphicalRuler : public QWidget
{
    Q_OBJECT

public:
    GraphicalRuler(QAbstractScrollArea *parent);
    virtual ~GraphicalRuler();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

signals:
    void sizeChanged(const QSize &newSize);

private slots:
    void setOffset(int value);

private:
    int offset;
    static qreal toMm();
};

#endif // GRAPHICALRULER_H