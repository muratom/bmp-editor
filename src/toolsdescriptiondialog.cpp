#include "toolsdescriptiondialog.h"
#include "ui_toolsdescriptiondialog.h"

ToolsDescriptionDialog::ToolsDescriptionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ToolsDescription)
{
    ui->setupUi(this);
}

ToolsDescriptionDialog::~ToolsDescriptionDialog()
{
    delete ui;
}
