#include "imageeditor.h"
#include "ui_imageeditor.h"

#include <QFileDialog> // Выбор файла для скачивания
#include <QDebug> // Отладочные сообщения
#include <QPixmap> // Добавление объекта QImage на scene
#include <QLabel> // Отображение координат нажатия мыши в Status bar
#include <QGraphicsScene>
#include <QGraphicsView> // Отображение изображения
#include <QMessageBox> // Информация о файле и о приложений
#include <QColorDialog> // Выбор цвета
#include <QMouseEvent> // Преобразование типов в event filter
#include <QTabWidget>

#include "bitmappicture.h"
#include "pixmapitem.h"
#include "rgb.h"
#include "rgbfilterdialog.h"
#include "drawsquaretools.h"
#include "swapareastools.h"
#include "toolsdescriptiondialog.h"

/*

TODO:
(+) Информация об изображении и приложений
(+) Выведение ошибки в случае неподдерживаемого формата изображения
(+) Убрать полные пути до директории
( ) Устранить утчеки памяти
(+) Отображать квадрат того цвета, который выбрал пользователь
( ) Изменить размер окна так, чтобы оно полностью вмещало изображение
(+) Если у 24-битного изображения может быть палитра цветов и при сохранении ее нужно учитывать, то нужно исправить функцию saveFile (BMP.cpp)

*/

/*
 * Пояснительная записка:
 * Функция swapAreas не поменяет области местами, если одна из координат вышла за пределы изображения
*/

ImageEditor::ImageEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ImageEditor)
{
    ui->setupUi(this);

    ui->statusbar->addWidget(mouseCoordinates);

    // Устанавливаем фильтр событий на виджеты, с которыми связаны действия
    for (auto widgetPtr : ui->actionRgbFilter->associatedWidgets())
        widgetPtr->installEventFilter(this);
    for (auto widgetPtr : ui->actionDrawSquare->associatedWidgets())
        widgetPtr->installEventFilter(this);
    for (auto widgetPtr : ui->actionSwapAreas->associatedWidgets())
        widgetPtr->installEventFilter(this);
    for (auto widgetPtr : ui->actionReplaceCommonColor->associatedWidgets())
        widgetPtr->installEventFilter(this);

    connect(ui->actionOpen, SIGNAL(triggered()),
           this, SLOT(open()));
    connect(ui->actionSave, SIGNAL(triggered()),
            this, SLOT(save()));
    connect(ui->actionClose, SIGNAL(triggered()),
            this, SLOT(close()));
    connect(ui->actionProperties, SIGNAL(triggered()),
            this, SLOT(properties()));
    connect(ui->actionToolsDescription, SIGNAL(triggered()),
            this, SLOT(toolsDescription()));
    connect(ui->actionAboutApplication, SIGNAL(triggered()),
            this, SLOT(about()));

    connect(ui->actionRgbFilter, SIGNAL(triggered()),
            this, SLOT(rgbFilter()));
    connect(ui->actionDrawSquare, SIGNAL(triggered()),
            this, SLOT(setDrawSquareMode()));
    connect(ui->actionSwapAreas, SIGNAL(triggered()),
            this, SLOT(setSwapAreasMode()));
    connect(ui->actionReplaceCommonColor, SIGNAL(triggered()),
            this, SLOT(replaceCommonColor()));
}

ImageEditor::~ImageEditor()
{
    delete ui;
    delete bmp;
    delete image;
    delete scene; // pixItem удаляется вместе со scene
    delete actionTools;
    // mouseCoordinates удаляется вместе с ImageEditor
}

void ImageEditor::updateImage()
{
    // Обновляем image
    for (int w = 0; w < image->width(); w++) {
        for (int h = 0; h < image->height(); h++) {
            QColor color(bmp->getPixelArray()[w][h].r, bmp->getPixelArray()[w][h].g, bmp->getPixelArray()[w][h].b);
            image->setPixel(w, h, color.rgb());
        }
    }

    // Обновляем отображаемое изображение
    pixItem->updateItem(*image);

    // Изображение изменено
    pixItem->setModified(true);
}

void ImageEditor::updateMouseCoordinates(QPointF mouseCoord)
{
    mouseCoordinates->setText("Coordinates: (" + QString::number(mouseCoord.x()) + ", " + QString::number(mouseCoord.y()) + ")");
}

bool ImageEditor::maybeSave()
{
    /*
     * Если изменении нет, пользователь сохранил изменения или не стал их сохранять, то возвращается true
     * Если же он передумал закрывать приложение, то возвращается false
     */
    if (pixItem != nullptr && pixItem->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, "Unsaved changes",
                                   "The image has been modified\n"
                                   "Do you want to save the changes?",
                                   QMessageBox::Ok | QMessageBox::Discard |
                                   QMessageBox::Cancel);
        if (ret == QMessageBox::Ok)
            return save();
        if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void ImageEditor::setSwapAreasMode()
{
    // Если изображения нет, то ничего не нужно делать
    if (bmp == nullptr) {
        QMessageBox::information(this, "Infomation", "Please, open the file.");
        return;
    }

    // Если режим "Перемещение областей" выбран, то не нужно сново добавлять панель инструментов и устанавливать данный режим
    if (pixItem->getMode() == MODE_SWAP_AREAS)
        return;

    // Если actionTools на что-то указывает, то удаляем данный объект
    if (actionTools != nullptr) {
        delete actionTools;
    }

    // Устанавливаем панель инструментов
    actionTools = new SwapAreasTools();
    addToolBarBreak(Qt::TopToolBarArea);
    addToolBar(Qt::TopToolBarArea, actionTools);

    // Включен режим "Перемещение областей"
    pixItem->setMode(MODE_SWAP_AREAS);
}

void ImageEditor::setDrawSquareMode()
{
    // Если изображения нет, то ничего не нужно делать
    if (bmp == nullptr) {
        QMessageBox::information(this, "Infomation", "Please, open the file.");
        return;
    }

    // Если режим "Рисование квадрата" выбран, то не нужно сново добавлять панель инструментов и устанавливать данный режим
    if (pixItem->getMode() == MODE_DRAW_SQUARE)
        return;

    // Если actionTools на что-то указывает, то удаляем данный объект
    if (actionTools != nullptr) {
        delete actionTools;
    }

    // Устанавливаем панель инструментов
    actionTools = new DrawSquareTools;
    addToolBarBreak(Qt::TopToolBarArea);
    addToolBar(Qt::TopToolBarArea, actionTools);

    // Включен режим "Рисование квадрата"
    pixItem->setMode(MODE_DRAW_SQUARE);
}

void ImageEditor::open()
{
    QString openFilePath = nullptr;
    openFilePath = QFileDialog::getOpenFileName(this, "Open file", "/home", "Images (*.bmp)");

    // Если ничего не было выбрано
    if (openFilePath.isNull())
        return;

    mouseCoordinates->setText("Coordinates: (0, 0)");

    // Сначала проверяем, поддерживается ли формат изображения
    BitmapPicture *tempBmp = new BitmapPicture(openFilePath.toStdString().c_str());
    if (tempBmp->isSupported() == false) {
        QMessageBox::warning(this, "Warning!",
                             "The format of this file is not supported by application.\n"
                             "Please, select another file.");
        delete tempBmp;
        return;
    }

    // Только если формат открывемого изображения поддерживается, удаляем предыдущее (даже если его не было)
    delete bmp;
    bmp = nullptr;

    delete image;
    image = nullptr;

    delete scene; // Также удаляется pixItem
    scene = nullptr;

    delete actionTools; // removeToolBar не удаляет сам объект, а прячет его
    actionTools = nullptr; // Чтобы при втором и последующих открытиях файлов, не происходило повторного освобождения памяти (при включении какого-либо режима)

    bmp = tempBmp;
    uint32_t width = bmp->getBih().width;
    uint32_t height = bmp->getBih().height;
    image = new QImage(width, height, QImage::Format_RGB888);

    // Копируем данные из BMP файла в image
    for (int w = 0; w < image->width(); w++) {
        for (int h = 0; h < image->height(); h++) {
            QColor color(bmp->getPixelArray()[w][h].r, bmp->getPixelArray()[w][h].g, bmp->getPixelArray()[w][h].b);
            image->setPixel(w, h, color.rgb());
        }
    }

    pixItem = new PixmapItem(*image);

    // Связываем нажатия мыши по изображению с соответствующими слотами
    connect(pixItem, SIGNAL(pointsSelected(QPointF, QPointF)),
            this, SLOT(drawSquare(QPointF, QPointF)));
    connect(pixItem, SIGNAL(pointsSelected(QPointF, QPointF)),
            this, SLOT(swapAreas(QPointF, QPointF)));
    connect(pixItem, SIGNAL(imageClicked(QPointF)),
            this, SLOT(updateMouseCoordinates(QPointF)));

    // Устанавливаем новое изображение
    scene = new QGraphicsScene(this);
    scene->addItem(pixItem);
    ui->graphicsView->setScene(scene);
}

bool ImageEditor::save()
{
    // Если изображения нет, то нечего сохранять
    if (bmp == nullptr) {
        QMessageBox::information(this, "Infomation", "Please, open the file.");
        return false;
    }

    const QString saveFilePath = QFileDialog::getSaveFileName(this, "Save file", "/home/ruslan/Programming/programs/bmp", "Images (*.bmp)");
    // Если пользователь ничего не выбрал
    if (saveFilePath == "")
        return false;

    bool isSaved = bmp->saveFile(saveFilePath.toStdString().c_str());
    if (isSaved) {
        pixItem->setModified(false);
    }
    return isSaved;
}

void ImageEditor::properties()
{
    // Если изображения нет, то инструменты недействительны
    if (bmp == nullptr) {
        QMessageBox::information(this, "Infomation", "Please, open the file.");
        return;
    }

    QMessageBox::about(this, "Image properties",
                       "Type: BMP file\n"
                       "File size: " + QString::number(bmp->getBfh().fileSize) + " bytes\n"
                       "Header size: " + QString::number(bmp->getBih().headerSize) + "\n"
                       "Width: " + QString::number(bmp->getBih().width) + "\n"
                       "Height: " + QString::number(bmp->getBih().height) + "\n"
                       "Planes: " + QString::number(bmp->getBih().planes) + "\n"
                       "Bits per pixel: " + QString::number(bmp->getBih().bitsPerPixel) + "\n"
                       "Compression: " + QString::number(bmp->getBih().compression) + "\n"
                       "Image size: " + QString::number(bmp->getBih().imageSize) + " bytes\n"
                       "Horizontal resolution: " + QString::number(bmp->getBih().xPixelsPerMeter) + "\n"
                       "Vertical resolution: " + QString::number(bmp->getBih().yPixelsPerMeter) + "\n"
                       "Colors in color table: " + QString::number(bmp->getBih().colorsInColorTable));
}

void ImageEditor::toolsDescription()
{
    ToolsDescriptionDialog tdd(this);
    tdd.exec();
}

void ImageEditor::about()
{
    QMessageBox::about(this, "ImageEditor",
                       "This application is the result of course work.\n\n"
                       "ImageEditor provides a BMP file editing interface.\n"
                       "Application support editing BMP files with following properties:\n"
                       "BITMAPINFO version: 3, 4 and 5 (but application actually can interact only with 3rd version's data);\n"
                       "Bits per pixel: 24;\n"
                       "Compression: 0.\n\n"
                       "Author: Muratov Ruslan, group 9303, 1st year student of SPbETU.\n"
                       "E-mail: rm070201wr@gmail.com");
}

void ImageEditor::rgbFilter()
{
    // Если изображения нет, то инструменты недействительны
    if (bmp == nullptr) {
        QMessageBox::information(this, "Infomation", "Please, open the file.");
        return;
    }

    rgbFilterDialog rgbFd(this);
    rgbFd.exec();
    compVal* compVal = rgbFd.getCompVal();

    // Если ничего не было выбрано
    if (compVal->component == COMPONENT_NULL) {
        return;
    }

    bmp->rgbFilter(compVal->component, compVal->value);

    // Обновление изображения
    updateImage();
}


void ImageEditor::drawSquare(QPointF p1, QPointF p2)
{
    // Если выбран другой режим, то функция не выполняется
    if (pixItem->getMode() != MODE_DRAW_SQUARE)
        return;

    // То, что файл открыт, проверяется в DrawingSquareOn
    DrawSquareTools *dst = dynamic_cast<DrawSquareTools*>(actionTools);

    RGB sideColor = {static_cast<uint8_t>(dst->sideColor.blue()),
                     static_cast<uint8_t>(dst->sideColor.green()),
                     static_cast<uint8_t>(dst->sideColor.red())};

    RGB fillColor = {0, 0, 0};
    if (dst->isFilledBox->isChecked()) {
        fillColor = {static_cast<uint8_t>(dst->fillColor.blue()),
                     static_cast<uint8_t>(dst->fillColor.green()),
                     static_cast<uint8_t>(dst->fillColor.red())};
    }

    bmp->drawSquare(Point(p1.x(), p1.y()), Point(p2.x(), p2.y()), dst->thicknessBox->value(), sideColor, dst->isFilledBox->isChecked(), fillColor);

    updateImage();
}

void ImageEditor::swapAreas(QPointF p1, QPointF p2)
{
    // Если выбран другой режим, то функция не выполняется
    if (pixItem->getMode() != MODE_SWAP_AREAS)
        return;

    // То, что файл открыт, проверяется в swapingAreasOn
    SwapAreasTools *sat = dynamic_cast<SwapAreasTools*>(actionTools);

    SwapType swapType = SWAP_DIAGONALLY;
    if (sat->getSwapTypeBox()->currentText() == "Clockwise")
        swapType = SWAP_CLOCKWISE;
    if (sat->getSwapTypeBox()->currentText() == "Contrclockwise")
        swapType = SWAP_CONTRCLOCKWISE;

    bmp->swapAreas(Point(p1.x(), p1.y()), Point(p2.x(), p2.y()), swapType);

    updateImage();
}

void ImageEditor::replaceCommonColor()
{
    // Если изображения нет, то инструменты недействительны
    if (bmp == nullptr) {
        QMessageBox::information(this, "Infomation", "Please, open the file.");
        return;
    }
    QColor color = QColorDialog::getColor(Qt::white, this, "Select color");
    if (!color.isValid())
        return;
    RGB replacmentPixel = {static_cast<uint8_t>(color.blue()),
                           static_cast<uint8_t>(color.green()),
                           static_cast<uint8_t>(color.red())};
    bmp->replaceCommonColor(replacmentPixel);
    updateImage();
}

bool ImageEditor::eventFilter(QObject *object, QEvent *event)
{
    // Если мы находимся в режиме "Рисование" и выбираем любое действие, то режим выключается. В ином случае ничего не происходит.
    if (event->type() == QEvent::MouseButtonPress &&
            dynamic_cast<QMouseEvent*>(event)->button() == Qt::LeftButton &&
            pixItem != nullptr &&
            pixItem->getMode() != MODE_NONE &&
            actionTools != nullptr) {
        removeToolBar(actionTools);
        pixItem->setMode(MODE_NONE);
    }
    return QMainWindow::eventFilter(object, event);
}

void ImageEditor::closeEvent(QCloseEvent *event)
{
    // Логика заложена в maybeSave()
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}
