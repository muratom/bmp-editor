#include "imageeditor.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ImageEditor w;
    w.show();
    return app.exec();
}
