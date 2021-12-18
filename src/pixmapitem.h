#ifndef PIXMAPITEM_H
#define PIXMAPITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QLabel>

#include "editmode.h"

class PixmapItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    PixmapItem(const QImage &image, QGraphicsItem *graphics_parent = nullptr, QObject *parent = nullptr);
    ~PixmapItem() = default;

    void updateItem(const QImage &image);

    const QPointF getPressPoint() { return pressPoint; } // Координаты нажатия кнопки мыши
    EditMode getMode() const { return mode; } // В каком режиме редактирования находится изображение
    bool isModified() const { return modified; } // Нужно ли спросить пользователя о сохранении изображения, если оно изменено

    void setMode(EditMode em) { mode = em; } // Установить режим редактирования
    void setModified(bool state) { modified = state; } // Изменено ли изображение

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

signals:
    void pointsSelected(QPointF point1, QPointF point2); // Выбраны точки для рисования
    void imageClicked(QPointF point); // Произошло нажатие на изображение

private:
    QPointF pressPoint;
    EditMode mode = MODE_NONE;
    bool modified = false;
};

#endif // PIXMAPITEM_H
