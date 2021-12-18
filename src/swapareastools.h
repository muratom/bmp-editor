#ifndef SWAPAREASTOOLS_H
#define SWAPAREASTOOLS_H

#include <QToolBar>
#include <QComboBox>

class SwapAreasTools : public QToolBar
{
    Q_OBJECT
public:
    SwapAreasTools();
    ~SwapAreasTools() = default; // Все виджеты связаны с tool bar, поэтому они удалятся вместе с ним

    QComboBox *getSwapTypeBox() { return swapTypeBox; }

private:
    QComboBox *swapTypeBox = new QComboBox(this);
};

#endif // SWAPAREASTOOLS_H
