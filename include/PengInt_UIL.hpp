#ifndef PENGINT_UIL_HPP
#define PENGINT_UIL_HPP

#include <cstdint>
#include <vector>
#include <string>

#include "raylib.h"


class UIElem;

std::vector<UIElem*> UIElems;

class UIElem {
public:
    Vector2 POS, SIZE;
    Color COL;
    UIElem(Vector2 p, Vector2 s, Color c) : POS(p), SIZE(s), COL(c) { UIElems.push_back(this); }
    UIElem(float x, float y, float w, float h, Color c) : POS({x, y}), SIZE({w, h}), COL(c) { UIElems.push_back(this); }
    UIElem(Vector4 r, Color c) : POS({r.x, r.y}), SIZE({r.z, r.w}), COL(c) { UIElems.push_back(this); }
};


class BTN;

std::vector<BTN*> BTNs;

class BTN : public UIElem {
public:
    BTN(Vector2 p, Vector2 s, Color c) : UIElem(p, s, c) { BTNs.push_back(this); }
    BTN(float x, float y, float w, float h, Color c) : UIElem(x, y, w, h, c) { BTNs.push_back(this); }
    BTN(Vector4 r, Color c) : UIElem(r, c) { BTNs.push_back(this); }
    virtual void OnClick() { }
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
    virtual void OnRun() { }
    virtual void OnUpdate_UI(float dt, float t) { }
    virtual void PreUpdate_UI(float dt, float t) { }
public:
    void Run() {
        OnRun();
        while (!WindowShouldClose()) {
            PreUpdate_UI(0, 0);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 c_pos = GetMousePosition();
                for (BTN* btn : BTNs) if (c_pos.x > btn->POS.x && c_pos.y > btn->POS.y && c_pos.x < btn->POS.x+btn->SIZE.x && c_pos.y < btn->POS.y+btn->SIZE.y) { btn->OnClick(); break; }
            }
            BeginDrawing();
                ClearBackground(WHITE);
                for (UIElem* e : UIElems) {
                    DrawRectangle(e->POS.x, e->POS.y, e->SIZE.x, e->SIZE.y, e->COL);
                }
            EndDrawing();
            OnUpdate_UI(0, 0);
        }
    }
};

#endif //PENGINT_UIL_HPP