#include "swapareastools.h"

#include <QLabel>

SwapAreasTools::SwapAreasTools()
{
    this->addWidget(new QLabel("Swap type: ", this));
    swapTypeBox->addItem("Diagonally");
    swapTypeBox->addItem("Clockwise");
    swapTypeBox->addItem("Contrclockwise");
    this->addWidget(swapTypeBox);
}
