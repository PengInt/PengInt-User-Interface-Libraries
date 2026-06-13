#ifndef PENGINT_UIL_HPP
#define PENGINT_UIL_HPP

#include <cstdint>
#include <vector>
#include <string>

#define GRAPHICS_API_OPENGL_43
#include <chrono>

#include "raylib.h"
#include "rlgl.h"
#include "external/glad.h"


Font Roboto_Mono;

class UIElement;
std::vector<UIElement*> UIElements;
class UIElement {
public:
    Vector2 POS, SIZE;
    Color COL;
    std::string Identifier;
    UIElement() : POS({0, 0}), SIZE({0, 0}), COL({0, 0, 0, 0}) { Identifier = ""; }
private: void Setup() { UIElements.push_back(this); Identifier = ""; }
    void Setup(std::string& id) { UIElements.push_back(this); Identifier = id; }
public:
    UIElement(Vector2 p, Vector2 s, Color c, std::string id) : POS(p), SIZE(s), COL(c) { Setup(id); }
    UIElement(float x, float y, float w, float h, Color c, std::string id) : POS({x, y}), SIZE({w, h}), COL(c) { Setup(id); }
    UIElement(Vector4 r, Color c, std::string id) : POS({r.x, r.y}), SIZE({r.z, r.w}), COL(c) { Setup(id); }
    UIElement(Vector2 p, Vector2 s, Color c) : POS(p), SIZE(s), COL(c) { Setup(); }
    UIElement(float x, float y, float w, float h, Color c) : POS({x, y}), SIZE({w, h}), COL(c) { Setup(); }
    UIElement(Vector4 r, Color c) : POS({r.x, r.y}), SIZE({r.z, r.w}), COL(c) { Setup(); }
    virtual void Draw() {
        DrawRectangle(POS.x, POS.y, SIZE.x, SIZE.y, COL);
    }
    virtual void Unload() {}
};


class UIButton;
std::vector<UIButton*> BTNs;
class UIButton : public UIElement {
public:
    UIButton() {}
private: void Setup() { BTNs.push_back(this); }
public:
    UIButton(Vector2 p, Vector2 s, Color c, std::string id) : UIElement(p, s, c, id) { Setup(); }
    UIButton(float x, float y, float w, float h, Color c, std::string id) : UIElement(x, y, w, h, c, id) { Setup(); }
    UIButton(Vector4 r, Color c, std::string id) : UIElement(r, c, id) { Setup(); }
    UIButton(Vector2 p, Vector2 s, Color c) : UIElement(p, s, c) { Setup(); }
    UIButton(float x, float y, float w, float h, Color c) : UIElement(x, y, w, h, c) { Setup(); }
    UIButton(Vector4 r, Color c) : UIElement(r, c) { Setup(); }
    virtual void OnClick() { std::cout << "A button was clicked (ID '" << Identifier << "'), but nothing happened, because the dev forgot to implement that part." << std::endl; }
};

template <typename T>
concept IsElementWithImage = requires(T o) { o.Source; };

class UIImage : public UIElement {
public:
    Texture Source;
    UIImage() {}
private: void Setup(std::string& fp) { Source = LoadTexture(fp.c_str()); }
public:
    UIImage(Vector2 p, Vector2 s, std::string fp, std::string id) : UIElement(p, s, {0, 0, 0, 0}, id) { Setup(fp); }
    UIImage(float x, float y, float w, float h, std::string fp, std::string id) : UIElement(x, y, w, h, {0, 0, 0, 0}, id) { Setup(fp); }
    UIImage(Vector4 r, std::string fp, std::string id) : UIElement(r, {0, 0, 0, 0}, id) { Setup(fp); }
    UIImage(Vector2 p, Vector2 s, std::string fp) : UIElement(p, s, {0, 0, 0, 0}) { Setup(fp); }
    UIImage(float x, float y, float w, float h, std::string fp) : UIElement(x, y, w, h, {0, 0, 0, 0}) { Setup(fp); }
    UIImage(Vector4 r, std::string fp) : UIElement(r, {0, 0, 0, 0}) { Setup(fp); }
    void Draw() override {
        DrawTexturePro(Source, {0, 0, (float) Source.width, (float) Source.height}, {POS.x, POS.y, SIZE.x, SIZE.y}, {0, 0}, 0, {255, 255, 255, 255});
    }
    void Unload() override {
        UnloadTexture(Source);
    }
};
class UIImageButton : public UIButton {
public:
    Texture Source;
    UIImageButton() {}
private: void Setup(std::string& fp) { Source = LoadTexture(fp.c_str()); }
public:
    UIImageButton(Vector2 p, Vector2 s, std::string fp, std::string id) : UIButton(p, s, {0, 0, 0, 0}, id) { Setup(fp); }
    UIImageButton(float x, float y, float w, float h, std::string fp, std::string id) : UIButton(x, y, w, h, {0, 0, 0, 0}, id) { Setup(fp); }
    UIImageButton(Vector4 r, std::string fp, std::string id) : UIButton(r, {0, 0, 0, 0}, id) { Setup(fp); }
    UIImageButton(Vector2 p, Vector2 s, std::string fp) : UIButton(p, s, {0, 0, 0, 0}) { Setup(fp); }
    UIImageButton(float x, float y, float w, float h, std::string fp) : UIButton(x, y, w, h, {0, 0, 0, 0}) { Setup(fp); }
    UIImageButton(Vector4 r, std::string fp) : UIButton(r, {0, 0, 0, 0}) { Setup(fp); }
    void Draw() override {
        DrawTexturePro(Source, {0, 0, (float) Source.width, (float) Source.height}, {POS.x, POS.y, SIZE.x, SIZE.y}, {0, 0}, 0, {255, 255, 255, 255});
    }
    void Unload() override {
        UnloadTexture(Source);
    }
};

class UIText : public UIElement {
public:
    Color TextColour;
    int TextSize;
    std::string Contents;
    UIText() {}
private: void Setup(Color tc, int ts, std::string& contents) { TextColour = tc; TextSize = ts; Contents = contents; }
public:
    UIText(Vector2 p, Vector2 s, Color tc, int ts, Color c, std::string contents, std::string id) : UIElement(p, s, c, id) { Setup(tc, ts, contents); }
    UIText(float x, float y, float w, float h, Color tc, int ts, Color c, std::string contents, std::string id) : UIElement(x, y, w, h, c, id) { Setup(tc, ts, contents); }
    UIText(Vector4 r, Color tc, int ts, Color c, std::string contents, std::string id) : UIElement(r, c, id) { Setup(tc, ts, contents); }
    UIText(Vector2 p, Vector2 s, Color tc, int ts, Color c, std::string contents) : UIElement(p, s, c) { Setup(tc, ts, contents); }
    UIText(float x, float y, float w, float h, Color tc, int ts, Color c, std::string contents) : UIElement(x, y, w, h, c) { Setup(tc, ts, contents); }
    UIText(Vector4 r, Color tc, int ts, Color c, std::string contents) : UIElement(r, c) { Setup(tc, ts, contents); }
    void Draw() override {
        DrawRectangle(POS.x, POS.y, SIZE.x, SIZE.y, COL);
        Vector2 TextVector2 = MeasureTextEx(Roboto_Mono, Contents.c_str(), TextSize, 0);
        DrawTextEx(Roboto_Mono, Contents.c_str(), {POS.x+(SIZE.x-TextVector2.x)/2, POS.y+(SIZE.y-TextVector2.y)/2}, TextSize, 0, TextColour);
    }
};
class UITextButton : public UIButton {
public:
    Color TextColour;
    int TextSize;
    std::string Contents;
    UITextButton() {}
private: void Setup(Color tc, int ts, std::string& contents) { TextColour = tc; TextSize = ts; Contents = contents; }
public:
    UITextButton(Vector2 p, Vector2 s, Color tc, int ts, Color c, std::string contents, std::string id) : UIButton(p, s, c, id) { Setup(tc, ts, contents); }
    UITextButton(float x, float y, float w, float h, Color tc, int ts, Color c, std::string contents, std::string id) : UIButton(x, y, w, h, c, id) { Setup(tc, ts, contents); }
    UITextButton(Vector4 r, Color tc, int ts, Color c, std::string contents, std::string id) : UIButton(r, c, id) { Setup(tc, ts, contents); }
    UITextButton(Vector2 p, Vector2 s, Color tc, int ts, Color c, std::string contents) : UIButton(p, s, c) { Setup(tc, ts, contents); }
    UITextButton(float x, float y, float w, float h, Color tc, int ts, Color c, std::string contents) : UIButton(x, y, w, h, c) { Setup(tc, ts, contents); }
    UITextButton(Vector4 r, Color tc, int ts, Color c, std::string contents) : UIButton(r, c) { Setup(tc, ts, contents); }
    void Draw() override {
        DrawRectangle(POS.x, POS.y, SIZE.x, SIZE.y, COL);
        Vector2 TextVector2 = MeasureTextEx(Roboto_Mono, Contents.c_str(), TextSize, 0);
        DrawTextEx(Roboto_Mono, Contents.c_str(), {POS.x+(SIZE.x-TextVector2.x)/2, POS.y+(SIZE.y-TextVector2.y)/2}, TextSize, 0, TextColour);
    }
};

class UIElementArray {
    bool VerticalStack;
    int Spacing;
    std::vector<UIElement*> Contents;
public:
    void UpdateSpacing() {
        if (Contents.size() == 0) return;
        Vector2 Offset = Contents[0]->POS;
        for (UIElement* e : Contents) {
            e->POS = Offset;
            if (VerticalStack) Offset.y += e->SIZE.y + Spacing;
            else Offset.x += e->SIZE.x + Spacing;
        }
    }
    UIElementArray() { VerticalStack = false; Spacing = 0; Contents = {}; }
    UIElementArray(bool orientation, int spacing) { VerticalStack = orientation; Spacing = spacing; }
    UIElementArray(bool orientation, int spacing, std::vector<UIElement*> preset) { VerticalStack = orientation; Spacing = spacing; Contents = preset; UpdateSpacing(); }
    void push_back(UIElement* ui_element) { Contents.push_back(ui_element); UpdateSpacing(); }
    UIElement* operator[](size_t i) { return Contents[i]; }
    const UIElement* operator[](size_t i) const { return Contents[i]; }
};

class Window {
public:
    uint16_t WIDTH, HEIGHT;
    bool CLEAR_BACKHROUND;
    Window(uint16_t w, uint16_t h) : WIDTH(w), HEIGHT(h) {
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(WIDTH, HEIGHT, "PengInt UI");
        Roboto_Mono = LoadFont("Roboto_Mono/RobotoMono-VariableFont_wght.ttf");
        SetExitKey(0);
        CLEAR_BACKHROUND = true;
    }
    Window(uint16_t w, uint16_t h, const std::string &title) : WIDTH(w), HEIGHT(h) {
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(WIDTH, HEIGHT, title.c_str());
        Roboto_Mono = LoadFont("Roboto_Mono/RobotoMono-VariableFont_wght.ttf");
        SetExitKey(0);
        CLEAR_BACKHROUND = true;
    }
protected:
    virtual void OnRun() { }
    virtual void OnEnd() { }
    virtual void OnUpdate_UI(float dt, float t) { }
    virtual void PreUpdate_UI(float dt, float t) { }
public:
    void Run() {
        OnRun();
        WIDTH = GetScreenWidth();
        HEIGHT = GetScreenHeight();
        while (!WindowShouldClose()) {
            if (IsWindowResized()) {
                WIDTH = GetScreenWidth();
                HEIGHT = GetScreenHeight();
            }
            float dt = GetFrameTime();
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 c_pos = GetMousePosition();
                for (UIButton* btn : BTNs) if (c_pos.x > btn->POS.x && c_pos.y > btn->POS.y && c_pos.x < btn->POS.x+btn->SIZE.x && c_pos.y < btn->POS.y+btn->SIZE.y) { btn->OnClick(); break; }
            }
            BeginDrawing();
                PreUpdate_UI(dt, 0);
                if (CLEAR_BACKHROUND) ClearBackground(WHITE);
                for (UIElement* e : UIElements) e->Draw();
            EndDrawing();
            OnUpdate_UI(dt, 0);
        }
        OnEnd();
        for (UIElement* e : UIElements) e->Unload();
        UnloadFont(Roboto_Mono);
        CloseWindow();
    }
};

#endif //PENGINT_UIL_HPP