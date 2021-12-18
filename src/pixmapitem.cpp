#include "pixmapitem.h"
#include "imageeditor.h"

#include <QDebug>
#include <QPixmap>

PixmapItem::PixmapItem(const QImage &image, QGraphicsItem *graphics_parent, QObject *parent)
    : QObject(parent),
      QGraphicsPixmapItem(QPixmap::fromImage(image), graphics_parent)
{}

void PixmapItem::updateItem(const QImage &image)
{
    setPixmap(QPixmap::fromImage(image));
}

void PixmapItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        pressPoint = event->pos();
        emit imageClicked(pressPoint);
    }
}

void PixmapItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && (mode == MODE_DRAW_SQUARE || mode == MODE_SWAP_AREAS)) {
        emit pointsSelected(pressPoint, event->pos());
        // Режим "Рисование" отключается в ImageEditor
    }
}
