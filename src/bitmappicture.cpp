#include "bitmappicture.h"
#include "rgb.h"
#include "components.h"
#include "swaptype.h"

#include <iostream>
#include <cstring>
#include <map>

constexpr int BMP_SIGNATURE = 19778;
constexpr int BITMAPCOREHEADER_SIZE = 12;
constexpr uint16_t SUPPORTED_BITS_PER_PIXEL = 24;
constexpr uint32_t SUPPORTED_COMPRESSION = 0;
constexpr uint32_t SUPPORTED_COLORS_IN_COLORTABLE = 0;

BitmapPicture::BitmapPicture(const char* picturePath)
{
    // Если формат загружаемого файла не поддерживается, то объект не будет инициализирован полностью
    supported = loadFile(picturePath);
}

BitmapPicture::~BitmapPicture()
{
    // Если формат файла не поддерживается, то память под массив пикселей не выделялась
    if (supported) {
        for (size_t i = 0; i < bih.width; i++) {
            delete [] pixelArray[i];
        }
        delete [] pixelArray;
    }
}

bool BitmapPicture::loadFile(const char *loadPicturePath)
{
    FILE* fp = fopen(loadPicturePath, "rb");
    if (fp == nullptr)
        return false;

    // Считываем сначала первый заголовок, чтобы узнать, имеет ли файл формат BMP
    if (fread(&bfh, sizeof(BitmapFileHeader), 1, fp) != 1) {
        return false;
    }
    if (bfh.signature != BMP_SIGNATURE)
        return false;

    // Считываем второй заголовок (при этом сначала проверяем версию заголовка)
    if (fread(&bitmapHeaderSize, sizeof(uint32_t), 1, fp) != 1) {
        return false;
    }
    fseek(fp, sizeof(BitmapFileHeader), SEEK_SET); // Возвращаемся назад, так как произошел сдвиг при считывании bitmapHeaderSize
    if (bitmapHeaderSize == BITMAPCOREHEADER_SIZE)
        return false;

    // Второй заголовок как минимум 3-ей версий (памяти выделено для хранения 3, 4 и 5 версий, то есть 124 байта)
    if (fread(&bih, bitmapHeaderSize, 1, fp) != 1) {
        return false;
    }

    // Проверяем количество битов на пиксель, сжатие изображения и наличие палитры
    if (bih.bitsPerPixel != SUPPORTED_BITS_PER_PIXEL ||
            bih.compression != SUPPORTED_COMPRESSION ||
            bih.colorsInColorTable != SUPPORTED_COLORS_IN_COLORTABLE) {
        return false;
    }

    // Сдвиг указателя в файле на начало массива пикселей
    fseek(fp, bfh.pixelArrayOffset, SEEK_SET);
    pixelArray = new RGB*[bih.width];
    for (size_t i = 0; i < bih.width; i++) {
        pixelArray[i] = new RGB[bih.height];
    }

    // Записываем массив пикселей в удобном формате (переворачиваем изображение и делаем индексацию вида [ширина][высота])
    for (size_t h = 0; h < bih.height; h++) {
        for (size_t w = 0; w < bih.width; w++) {
            pixelArray[w][bih.height - 1 - h].b = fgetc(fp);
            pixelArray[w][bih.height - 1 - h].g = fgetc(fp);
            pixelArray[w][bih.height - 1 - h].r = fgetc(fp);
        }
    }

    fclose(fp);
    return true;
}

bool BitmapPicture::saveFile(const char *saveFilePath)
{
    FILE *fp = fopen(saveFilePath, "wb");
    if (fp == nullptr)
        return false;

    uint32_t height = bih.height;
    uint32_t width = bih.width;

    fwrite(&bfh, sizeof(BitmapFileHeader), 1, fp);
    fwrite(&bih, bitmapHeaderSize, 1, fp);

    // Предполагаем, что в 24-битных изображениях нет палитры (данное условие проверяется), поэтому данная строка бесполезна:
    fseek(fp, bfh.pixelArrayOffset, SEEK_SET);

    for (size_t h = 0; h < height; h++) {
        // Сохраняем массивы пикселей в соответствии с форматом BMP (вверх ногами)
        for (size_t w = 0; w < width; w++) {
            fwrite(&pixelArray[w][height - 1 - h], sizeof(RGB), 1, fp);
        }
        for (size_t i = 0; i < width % 4; i++)
            // Заполнение нулями строки, чтобы их длина в байтах делилась на 4 (24 бита на пиксель => width % 4)
            fputc(0, fp);
    }
    fclose(fp);
    return true;
}

// 1-ая подзадача
void BitmapPicture::rgbFilter(Components component, uint8_t value)
{
    for (size_t w = 0; w < bih.width; w++) {
        for (size_t h = 0; h < bih.height; h++) {
            switch(component) {
                case COMPONENT_RED:
                    pixelArray[w][h].r = value;
                    break;
                case COMPONENT_GREEN:
                    pixelArray[w][h].g = value;
                    break;
                case COMPONENT_BLUE:
                    pixelArray[w][h].b = value;
                    break;
                default:
                    // Данный случай учитывается в единственном (на данный момент) вызове функции в ImageEditor
                    return;
            }
        }
    }
}

// 2-ая подзадача
void BitmapPicture::drawSquare(Point p1, Point p2, long thickness, RGB sideColor, bool isFilled, RGB fillColor)
{
    // Вычисляем длину стороны квадрата с учетом того, что координаты точке могут быть любыми
    long sideLen = std::min(std::abs(p1.x - p2.x), std::abs(p1.y - p2.y));

    // Выбираем координты левого верхнего угла (точки p1 и p2 могут произвольно находится друг относительно друга)
    Point topLeft = {
        std::min(p1.x, p2.x),
        std::min(p1.y, p2.y)
    };

    // Длина стороны = количество пикселей на одну сторону
    drawSides(topLeft, sideLen, thickness, sideColor);
    if (isFilled) {
        fillSquare(topLeft, sideLen, thickness, fillColor);
    }
}

// 3-ья подзадача
void BitmapPicture::swapAreas(Point p1, Point p2, SwapType swapType)
{
    // Учитываем, что две точки могут быть выбраны произвольно
    Point topLeft = {
        std::min(p1.x, p2.x),
        std::min(p1.y, p2.y)
    };

    Point bottomRight = {
        std::max(p1.x, p2.x),
        std::max(p1.y, p2.y)
    };

    // Если одна из точке вышла за пределы изображения, то ничего не происходит
    if (topLeft.x < 0 || topLeft.y < 0 || bottomRight.x >= bih.width || bottomRight.y >= bih.height)
        return;

    // Ширина и высота одной из 4 областей
    long areaWidth = std::abs(p2.x - p1.x) + 1;
    long areaHeight = std::abs(p2.y - p1.y) + 1;

    // Смещение для случая, когда количество пикслей нечетное
    unsigned char widthStep = areaWidth % 2;
    unsigned char heightStep = areaHeight % 2;

    if (swapType == SWAP_DIAGONALLY) {
        for (long i = 0; i < areaWidth / 2; i++) {
            for (long j = 0; j < areaHeight / 2; j++) {
                // Меняем местами top left и bottom right области
                swapPixels(pixelArray[topLeft.x + i][topLeft.y + j], pixelArray[topLeft.x + areaWidth / 2 + widthStep + i][topLeft.y + areaHeight / 2 + heightStep + j]);
                // Меняем местами bottom left и top right области
                swapPixels(pixelArray[topLeft.x + i][topLeft.y + areaHeight / 2 + heightStep + j], pixelArray[topLeft.x + areaWidth / 2 + widthStep + i][topLeft.y + j]);
            }
        }
        return;
    }

    // top left - буфер для каждой области
    if (swapType == SWAP_CLOCKWISE) {
        for (long i = 0; i < areaWidth / 2; i++) {
            for (long j = 0; j < areaHeight / 2; j++) {
                // Меняем местами top left и top right области
                swapPixels(pixelArray[topLeft.x + i][topLeft.y + j], pixelArray[topLeft.x + areaWidth / 2 + widthStep + i][topLeft.y + j]);
                // Меняем местами top left и bottom right области
                swapPixels(pixelArray[topLeft.x + i][topLeft.y + j], pixelArray[topLeft.x + areaWidth / 2 + widthStep + i][topLeft.y + areaHeight / 2 + heightStep + j]);
                // Меняем местами top left и bottom left области
                swapPixels(pixelArray[topLeft.x + i][topLeft.y + j], pixelArray[topLeft.x + i][topLeft.y + areaHeight / 2 + heightStep + j]);
            }
        }
        return;
    }
    // top left - буфер для каждой области
    if (swapType == SWAP_CONTRCLOCKWISE) {
        for (long i = 0; i < areaWidth / 2; i++) {
            for (long j = 0; j < areaHeight / 2; j++) {
                // Меняем местами top left и bottom left области
                swapPixels(pixelArray[topLeft.x + i][topLeft.y + j], pixelArray[topLeft.x + i][topLeft.y + areaHeight / 2 + heightStep + j]);
                // Меняем местами top left и top right области
                swapPixels(pixelArray[topLeft.x + i][topLeft.y + j], pixelArray[topLeft.x + areaWidth / 2 + widthStep + i][topLeft.y + j]);
                // Меняем местами top right и bottom right области
                swapPixels(pixelArray[topLeft.x + areaWidth / 2 + widthStep + i][topLeft.y + j], pixelArray[topLeft.x + areaWidth / 2 + widthStep + i][topLeft.y + areaHeight / 2 + heightStep + j]);
            }
        }
        return;
    }
}

// 4-ая подзадача
void BitmapPicture::replaceCommonColor(const RGB &replacementColor)
{
    using std::map;
    auto *colorCounter = new map<RGB, long>;
    uint32_t width = bih.width;
    uint32_t height = bih.height;

    // Заполнение словаря pixelCounter;
    for (size_t w = 0; w < width; w++) {
        for (size_t h = 0; h < height; h++) {
            if (colorCounter->find(pixelArray[w][h]) == colorCounter->end())
                (*colorCounter)[pixelArray[w][h]] = 0;
            (*colorCounter)[pixelArray[w][h]]++;
        }
    }

    // Нахождение наиболее часто встречаемый цвет
    RGB commonColor = {0, 0, 0};
    long maxFrequency = 0;
    for (const std::pair<RGB, long> currentPair: *colorCounter) {
        if (currentPair.second > maxFrequency) {
            commonColor = currentPair.first;
            maxFrequency = currentPair.second;
        }
    }

    // Замена наиболее часто встречаемый цвет на выбранный пользователем
    for (size_t w = 0; w < width; w++) {
        for (size_t h = 0; h < height; h++) {
            if (pixelArray[w][h] == commonColor) {
                pixelArray[w][h] = replacementColor;
            }
        }
    }
    delete colorCounter;
}

void BitmapPicture::drawStraightLine(Point startPoint, Point endPoint, RGB lineColor)
{
    // x - ширина, y - высота
    // Если координты прямой выходят за пределы изображения, то не рисуем ее

    if (startPoint.x == endPoint.x && startPoint.x >= 0 && startPoint.x < bih.width) {
        for (long y = startPoint.y; y <= endPoint.y; y++) {
            if (y >= 0 && y < bih.height)
                pixelArray[startPoint.x][y] = lineColor;
        }
    }
    if (startPoint.y == endPoint.y && startPoint.y >= 0 && startPoint.y < bih.height) {
        for (long x = startPoint.x; x <= endPoint.x; x++) {
            if (x >= 0 && x < bih.width)
                pixelArray[x][startPoint.y] = lineColor;
        }
    }
}

void BitmapPicture::drawSides(Point topLeft, long sideLen, long thickness, RGB sideColor)
{
    long outStep = thickness / 2;
    long inStep = (thickness - 1) / 2;
    for (long curLine = 0; curLine < thickness; curLine++) {
        drawStraightLine(Point(topLeft.x - outStep + curLine, topLeft.y - outStep),
                         Point(topLeft.x - outStep + curLine, topLeft.y + sideLen - 1 + outStep), sideColor);

        drawStraightLine(Point(topLeft.x - outStep, topLeft.y - outStep + curLine),
                         Point(topLeft.x + sideLen - 1 + outStep, topLeft.y - outStep + curLine), sideColor);

        drawStraightLine(Point(topLeft.x - outStep, topLeft.y + sideLen - 1 - inStep + curLine),
                         Point(topLeft.x + sideLen - 1 + outStep, topLeft.y + sideLen - 1 - inStep + curLine), sideColor);

        drawStraightLine(Point(topLeft.x + sideLen - 1 - inStep + curLine, topLeft.y - outStep),
                         Point(topLeft.x + sideLen - 1 - inStep + curLine, topLeft.y + sideLen - 1 + outStep), sideColor);
    }
}

void BitmapPicture::fillSquare(Point topLeft, long sideLen, long thickness,RGB fillColor)
{
    long inStep = (thickness - 1) / 2;
    // Длина стороны = количеству пикселей на одну сторону => inStep + 1
    // signed, так как иначе не учитывается случай, когда internalSideLen < 0
    long internalSideLen = sideLen - 2 * (inStep + 1);

    for (long curLine = 0; curLine < internalSideLen; curLine++) {
        drawStraightLine(Point(topLeft.x + inStep + 1 + curLine, topLeft.y + inStep + 1), Point(topLeft.x + inStep + 1 + curLine, topLeft.y + inStep + internalSideLen), fillColor);
    }
}

void BitmapPicture::swapPixels(RGB &p1, RGB &p2)
{
    RGB temp = p1;
    p1 = p2;
    p2 = temp;
}
