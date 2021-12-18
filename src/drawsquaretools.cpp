#include "drawsquaretools.h"

#include <QLabel>
#include <QDebug>
#include <QColorDialog>

constexpr int MAX_THICKNESS = 100;

DrawSquareTools::DrawSquareTools()
{
//    this->addWidget(new QLabel("Side length: ", this));
//    // Максимальное допустимое значение устанаваливается в ImageEditor (т.е. зависит от изображения)
//    sideLenBox->setMinimum(1);
//    sideLenBox->setValue(1);
//    this->addWidget(sideLenBox);
//    this->addSeparator();

    this->addWidget(new QLabel("Thickness: ", this));
    thicknessBox->setRange(1, MAX_THICKNESS);
    thicknessBox->setValue(1);
    this->addWidget(thicknessBox);
    this->addSeparator();

    // Добавляем labels и buttons для цвета стороны и заливки
    this->addWidget(new QLabel("Side color: ", this));
    this->addWidget(btn_sideColor);
    btn_sideColor->setStyleSheet("background-color: black");
    this->addSeparator();

    isFilledBox->setChecked(false);
    this->addWidget(isFilledBox);
    this->addWidget(new QLabel("Fill color: ", this));
    this->addWidget(btn_fillColor);
    btn_fillColor->setStyleSheet("background-color: black");
    this->addSeparator();

    connect(btn_sideColor, SIGNAL(pressed()),
            this, SLOT(selectSideColor()));
    connect(btn_fillColor, SIGNAL(pressed()),
            this, SLOT(selectFillColor()));
}

void DrawSquareTools::selectSideColor()
{
    QColor temp_color = QColorDialog::getColor(Qt::white, this, "Select side color");
    if (temp_color.isValid()) {
        sideColor = temp_color;
        btn_sideColor->setStyleSheet(QString("background-color: " + sideColor.name()));
    }
}

void DrawSquareTools::selectFillColor()
{
    QColor temp_color = QColorDialog::getColor(Qt::white, this, "Select side color");
    if (temp_color.isValid()) {
        fillColor = temp_color;
        btn_fillColor->setStyleSheet(QString("background-color: " + fillColor.name()));
    }
}

