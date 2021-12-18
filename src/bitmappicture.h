#ifndef BITMAPPICTURE_H
#define BITMAPPICTURE_H

#include <cstdint>
#include <cinttypes>

#include "rgb.h"
#include "components.h"
#include "swaptype.h"
#include "point.h"

#pragma pack (push, 1)

typedef struct
{
    uint16_t signature;
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t pixelArrayOffset;
} BitmapFileHeader;

#pragma pack (pop)

typedef struct
{
    uint32_t headerSize;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t imageSize;
    uint32_t xPixelsPerMeter;
    uint32_t yPixelsPerMeter;
    uint32_t colorsInColorTable;
    uint32_t importantColorCount;
    uint8_t tail[84];
} BitmapInfoHeader;


class BitmapPicture
{
public:
    BitmapPicture(const char* picturePath);
    ~BitmapPicture();

    BitmapFileHeader const  &getBfh() { return bfh; }
    BitmapInfoHeader const &getBih() { return bih; }
    const RGB *const *getPixelArray() { return pixelArray; }
    bool isSupported() { return supported; }

    bool loadFile(const char* loadFilePath);
    bool saveFile(const char* saveFilePath);

    // Основные функции
    void rgbFilter(Components component, uint8_t value);
    void drawSquare(Point p1, Point p2, long thickness, RGB sideColor, bool isFilled, RGB fillColor);
    void swapAreas(Point p1, Point p2, SwapType swapType = SWAP_DIAGONALLY);
    void replaceCommonColor(const RGB &replacementColor);

    // Вспомогательные функции
    void drawStraightLine(Point startPoint, Point endPoint, RGB lineColor = {0, 0, 0});
    void drawSides(Point topLeft, long sideLen, long thickness, RGB sideColor = {0, 0, 0});
    void fillSquare(Point topLeft, long sideLen, long thickness,RGB fillColor);
    void swapPixels(RGB &p1, RGB &p2);

    void printFileHeader();
    void printInfoHeader();

private:
    BitmapFileHeader bfh;
    BitmapInfoHeader bih;
    RGB **pixelArray; // Массив пикселей хранится не в соответствии с форматом BMP
    uint32_t bitmapHeaderSize;
    bool supported = false;
};

#endif // BITMAPPICTURE_H
