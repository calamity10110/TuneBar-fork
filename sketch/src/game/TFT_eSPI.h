#ifndef _TFT_ESPI_MOCK_H_
#define _TFT_ESPI_MOCK_H_

#include <stdint.h>
#include <stddef.h>

class TFT_eSprite {
public:
    uint16_t* buffer;
    int _width;
    int _height;

    TFT_eSprite(void* dummy);
    void createSprite(int w, int h);
    void fillRect(int x, int y, int w, int h, uint16_t color);
    void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color);
    void drawPixel(int x, int y, uint16_t color);
    void pushImage(int x, int y, int w, int h, const uint16_t *data);
    void setSwapBytes(bool swap);
    void pushSprite(int x, int y);
    void pushToSprite(TFT_eSprite* spr, int x, int y);
    void setColorDepth(int d);
    void setAttribute(int attr, bool v);

    // Some basic text drawing (dummy implementation or simple one)
    void setTextSize(int size);
    void setTextColor(uint16_t color, uint16_t bg = 0);
    void setCursor(int x, int y);
    void print(const char* text);
    void print(int val);
    void print(float val, int decimals = 2);
    void fillCircle(int x, int y, int r, uint16_t color);
    void drawCircle(int x, int y, int r, uint16_t color);
    void drawLine(int x0, int y0, int x1, int y1, uint16_t color);
    void drawFastHLine(int x, int y, int w, uint16_t color);
    void drawRect(int x, int y, int w, int h, uint16_t color);
    void fillSprite(uint16_t color);
    void fillEllipse(int x, int y, int rx, int ry, uint16_t color);
};

class TFT_eSPI {
public:
    TFT_eSPI() {}
    void begin() {}
    void setRotation(int r) {}
    void fillScreen(uint16_t color) {}
};

#define TFT_BLACK       0x0000
#define TFT_NAVY        0x000F
#define TFT_DARKGREEN   0x03E0
#define TFT_DARKCYAN    0x03EF
#define TFT_MAROON      0x7800
#define TFT_PURPLE      0x780F
#define TFT_OLIVE       0x7BE0
#define TFT_LIGHTGREY   0xC618
#define TFT_DARKGREY    0x7BEF
#define TFT_BLUE        0x001F
#define TFT_GREEN       0x07E0
#define TFT_CYAN        0x07FF
#define TFT_RED         0xF800
#define TFT_MAGENTA     0xF81F
#define TFT_YELLOW      0xFFE0
#define TFT_WHITE       0xFFFF
#define TFT_ORANGE      0xFDA0
#define TFT_GREENYELLOW 0xAFE5
#define TFT_PINK        0xF81F

#define PSRAM_ENABLE 1

#endif
