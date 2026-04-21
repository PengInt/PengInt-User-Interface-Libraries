#ifndef PENGINT_UIL_HPP
#define PENGINT_UIL_HPP

#include <cstdint>
#include <vector>
#include <string>

#include "raylib.h"


class UIElem {
    Vector2 POS, SIZE;
    Color COL;
    UIElem(Vector2 p, Vector2 s, Color c) : POS(p), SIZE(s), COL(c) { }
    UIElem(float x, float y, float w, float h, Color c) : POS({x, y}), SIZE({w, h}), COL(c) { }
    UIElem(Vector4 r, Color c) : POS({r.x, r.y}), SIZE({r.z, r.w}), COL(c) { }
};

class Window {
public:
    uint16_t WIDTH, HEIGHT;
    Window(uint16_t w, uint16_t h) : WIDTH(w), HEIGHT(h) {
        InitWindow(WIDTH, HEIGHT, "PengInt UI");
    }
    Window(uint16_t w, uint16_t h, const std::string &title) : WIDTH(w), HEIGHT(h) {
        InitWindow(WIDTH, HEIGHT, title.c_str());
    }
protected:
    virtual void OnUpdate_UI(float dt, float t) { }
public:
    void Run() {
        while (!WindowShouldClose()) {
            BeginDrawing();
                ClearBackground(WHITE);
            EndDrawing();
            OnUpdate_UI(0, 0);
        }
    }
};

#endif //PENGINT_UIL_HPP