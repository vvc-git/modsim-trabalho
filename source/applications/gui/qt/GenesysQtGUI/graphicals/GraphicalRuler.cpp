#include <graphicals/GraphicalRuler.h>

GraphicalRuler::GraphicalRuler(QAbstractScrollArea *parent = nullptr) : QWidget(parent), offset(0)
{
    setFixedSize(40, parent->height());
    move(0, 40);
    connect(parent->verticalScrollBar(), &QScrollBar::valueChanged, this, &GraphicalRuler::setOffset);
}

GraphicalRuler::~GraphicalRuler() {}

void GraphicalRuler::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.translate(0, -offset);
    qreal const heightMm = height() * toMm();
    painter.setFont(font());
    QFontMetrics fm(font());
    for (int position = 0; position < heightMm; ++position)
    {
        int const positionInPix = int(position / toMm());
        if (position % 10 == 0)
        {
            if (position != 0)
            {
                QString const txt = QString::number(position);
                QRectF txtRect = fm.boundingRect(txt).translated(0, positionInPix);
                txtRect.translate(0, txtRect.height() / 2);
                painter.drawText(txtRect, txt);
            }
            painter.drawLine(width() - 15, positionInPix, width(), positionInPix);
        }
        else
        {
            painter.drawLine(width() - 10, positionInPix, width(), positionInPix);
        }
    }
}

void GraphicalRuler::resizeEvent(QResizeEvent *event)
{

    int const maximumMm = event->size().height() * toMm();
    QFontMetrics fm(font());
    int w = fm.width(QString::number(maximumMm)) + 20;
    if (w != event->size().width())
    {
        QSize const newSize(w, event->size().height());
        sizeChanged(newSize);
        return setFixedSize(newSize);
    }
    return QWidget::resizeEvent(event);
}

void GraphicalRuler::setOffset(int value)
{
    offset = value;
    update();
}

qreal GraphicalRuler::toMm()
{
    if (QApplication::screens().isEmpty())
    {
        // Não há telas disponíveis, retorne um valor padrão
        return 1.0;
    }

    QScreen *primaryScreen = QApplication::primaryScreen();
    qreal logicalDpiY = primaryScreen->logicalDotsPerInchY();

    return 25.4 / logicalDpiY;
}