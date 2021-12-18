#ifndef RGBFILTERDIALOG_H
#define RGBFILTERDIALOG_H

#include <QDialog>
#include "components.h"

struct compVal {
    Components component;
    uint8_t value;
};

namespace Ui {
class rgbFilterDialog;
}

class rgbFilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit rgbFilterDialog(QWidget* parent = nullptr);
    ~rgbFilterDialog();

    struct compVal* getCompVal();

public slots:
    void readData();

private:
    Ui::rgbFilterDialog* ui;
    struct compVal* compVal;
};

#endif // RGBFILTERDIALOG_H
