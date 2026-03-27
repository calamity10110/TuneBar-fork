#include "TFT_eSPI.h"
#include <string.h>
#include <stdlib.h>
#include "esp_heap_caps.h"
#include <Arduino.h>

// Custom sprite implementation mapped to a buffer
TFT_eSprite::TFT_eSprite(void* dummy) : buffer(NULL), _width(0), _height(0) {}

void TFT_eSprite::createSprite(int w, int h) {
    _width = w;
    _height = h;
    if (buffer) {
        free(buffer);
    }
    buffer = (uint16_t*)heap_caps_malloc(w * h * sizeof(uint16_t), MALLOC_CAP_SPIRAM);
    if (!buffer) {
        buffer = (uint16_t*)malloc(w * h * sizeof(uint16_t));
    }
}

void TFT_eSprite::fillRect(int x, int y, int w, int h, uint16_t color) {
    if (!buffer) return;
    int endX = x + w;
    int endY = y + h;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (endX > _width) endX = _width;
    if (endY > _height) endY = _height;

    uint16_t swapped_color = (color >> 8) | (color << 8);

    for (int j = y; j < endY; j++) {
        for (int i = x; i < endX; i++) {
            buffer[j * _width + i] = swapped_color;
        }
    }
}

void TFT_eSprite::drawPixel(int x, int y, uint16_t color) {
    if (!buffer) return;
    if (x >= 0 && x < _width && y >= 0 && y < _height) {
        buffer[y * _width + x] = (color >> 8) | (color << 8);
    }
}

void TFT_eSprite::fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
    if (!buffer) return;
    int a, b, y, last;
    if (y0 > y1) { int t=y0; y0=y1; y1=t; t=x0; x0=x1; x1=t; }
    if (y1 > y2) { int t=y1; y1=y2; y2=t; t=x1; x1=x2; x2=t; }
    if (y0 > y1) { int t=y0; y0=y1; y1=t; t=x0; x0=x1; x1=t; }

    if (y0 == y2) {
        a = b = x0;
        if (x1 < a)      a = x1;
        else if (x1 > b) b = x1;
        if (x2 < a)      a = x2;
        else if (x2 > b) b = x2;
        fillRect(a, y0, b - a + 1, 1, color);
        return;
    }

    int dx01 = x1 - x0, dy01 = y1 - y0;
    int dx02 = x2 - x0, dy02 = y2 - y0;
    int dx12 = x2 - x1, dy12 = y2 - y1;
    int32_t sa = 0, sb = 0;

    if (y1 == y2) last = y1;
    else last = y1 - 1;

    for (y = y0; y <= last; y++) {
        a = x0 + sa / dy01;
        b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        if (a > b) { int t=a; a=b; b=t; }
        fillRect(a, y, b - a + 1, 1, color);
    }

    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for (; y <= y2; y++) {
        a = x1 + sa / dy12;
        b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        if (a > b) { int t=a; a=b; b=t; }
        fillRect(a, y, b - a + 1, 1, color);
    }
}

void TFT_eSprite::pushImage(int x, int y, int w, int h, const uint16_t *data) {
    if (!buffer) return;
    int ptr = 0;
    for (int j = y; j < y + h; j++) {
        for (int i = x; i < x + w; i++) {
            if (i >= 0 && i < _width && j >= 0 && j < _height) {
                uint16_t color = data[ptr];
                buffer[j * _width + i] = color;
            }
            ptr++;
        }
    }
}

void TFT_eSprite::setSwapBytes(bool swap) {}
void TFT_eSprite::pushSprite(int x, int y) {}
void TFT_eSprite::pushToSprite(TFT_eSprite* spr, int x, int y) {
    if (!buffer || !spr || !spr->buffer) return;
    for (int j = 0; j < _height; j++) {
        for (int i = 0; i < _width; i++) {
            int targetX = x + i;
            int targetY = y + j;
            if (targetX >= 0 && targetX < spr->_width && targetY >= 0 && targetY < spr->_height) {
                // simple blit
                spr->buffer[targetY * spr->_width + targetX] = buffer[j * _width + i];
            }
        }
    }
}
void TFT_eSprite::setColorDepth(int d) {}
void TFT_eSprite::setAttribute(int attr, bool v) {}
void TFT_eSprite::setTextSize(int size) {}
void TFT_eSprite::setTextColor(uint16_t color, uint16_t bg) {}
void TFT_eSprite::setCursor(int x, int y) {}
void TFT_eSprite::print(const char* text) {}
void TFT_eSprite::print(int val) {}
void TFT_eSprite::print(float val, int decimals) {}
void TFT_eSprite::fillCircle(int x, int y, int r, uint16_t color) {}
void TFT_eSprite::drawCircle(int x, int y, int r, uint16_t color) {}
void TFT_eSprite::drawLine(int x0, int y0, int x1, int y1, uint16_t color) {}
void TFT_eSprite::drawFastHLine(int x, int y, int w, uint16_t color) { fillRect(x,y,w,1,color); }
void TFT_eSprite::drawRect(int x, int y, int w, int h, uint16_t color) {
    fillRect(x,y,w,1,color);
    fillRect(x,y+h-1,w,1,color);
    fillRect(x,y,1,h,color);
    fillRect(x+w-1,y,1,h,color);
}
void TFT_eSprite::fillSprite(uint16_t color) { fillRect(0,0,_width,_height,color); }
void TFT_eSprite::fillEllipse(int x, int y, int rx, int ry, uint16_t color) {}
