#ifndef IMAGEEDITOR_H
#define IMAGEEDITOR_H

#include <QMainWindow>
#include <QImage>
#include <QLabel>
#include <QGraphicsScene>

#include "bitmappicture.h"
#include "pixmapitem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ImageEditor; }
QT_END_NAMESPACE

class ImageEditor : public QMainWindow
{
    Q_OBJECT

public:
    ImageEditor(QWidget *parent = nullptr);
    ~ImageEditor();

public slots:
    void open();
    bool save();
    void properties();
    void toolsDescription();
    void about();

    void updateImage();
    void updateMouseCoordinates(QPointF mouseCoord);
    bool maybeSave();

    void setDrawSquareMode();
    void setSwapAreasMode();

    void rgbFilter();
    void drawSquare(QPointF p1, QPointF p2);
    void swapAreas(QPointF p1, QPointF p2);
    void replaceCommonColor();

    bool eventFilter(QObject *object, QEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    Ui::ImageEditor *ui;
    BitmapPicture *bmp = nullptr;
    QImage *image = nullptr;
    PixmapItem *pixItem = nullptr;
    QGraphicsScene *scene = nullptr;
    QLabel *mouseCoordinates = new QLabel("Coordinates: (0, 0)", this);
    QToolBar *actionTools = nullptr;
};

#endif // IMAGEEDITOR_H
