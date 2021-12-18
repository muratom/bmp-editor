#ifndef TOOLSDESCRIPTIONDIALOG_H
#define TOOLSDESCRIPTIONDIALOG_H

#include <QDialog>

namespace Ui {
class ToolsDescription;
}

class ToolsDescriptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ToolsDescriptionDialog(QWidget *parent = nullptr);
    ~ToolsDescriptionDialog();

private:
    Ui::ToolsDescription *ui;
};

#endif // TOOLSDESCRIPTIONDIALOG_H
