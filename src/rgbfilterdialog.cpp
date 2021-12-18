#include "rgbfilterdialog.h"
#include "ui_rgbfilterdialog.h"

rgbFilterDialog::rgbFilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::rgbFilterDialog),
    compVal(new struct compVal)
{
    ui->setupUi(this);

    // Ничего не выбрано
    compVal->component = COMPONENT_NULL;

    connect(ui->buttonBox, SIGNAL(accepted()),
            this, SLOT(readData()));
    connect(ui->buttonBox, SIGNAL(rejected()),
            this, SLOT(close()));
}

rgbFilterDialog::~rgbFilterDialog()
{
    delete compVal;
    delete ui;
}

struct compVal* rgbFilterDialog::getCompVal() {
    return compVal;
}

void rgbFilterDialog::readData() {
    if (ui->comboBox_Component->currentText() == "Red")
        compVal->component = COMPONENT_RED;
    else
        if (ui->comboBox_Component->currentText() == "Green")
            compVal->component = COMPONENT_GREEN;
        else
            if (ui->comboBox_Component->currentText() == "Blue")
                compVal->component = COMPONENT_BLUE;
    compVal->value = ui->spinBox_Value->value();
    close();
}
