#ifndef DRAWSQUARETOOLS_H
#define DRAWSQUARETOOLS_H

#include <QToolBar>
#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <QColor>

class DrawSquareTools : public QToolBar
{
    Q_OBJECT

public:
    DrawSquareTools();
    ~DrawSquareTools() = default; //Все виджеты связаны с tool bar, поэтому они удалятся вместе с ним

public slots:
    void selectSideColor();
    void selectFillColor();

public:
    QSpinBox *thicknessBox = new QSpinBox(this);
    QColor sideColor = QColor(0, 0, 0);
    QColor fillColor = QColor(0, 0, 0);
    QPushButton *btn_sideColor = new QPushButton(this);
    QPushButton *btn_fillColor = new QPushButton(this);
    QCheckBox *isFilledBox = new QCheckBox("Filled", this);
};

#endif // DRAWSQUARETOOLS_H
