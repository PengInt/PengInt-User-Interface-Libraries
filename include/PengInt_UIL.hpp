#ifndef PENGINT_UIL_HPP
#define PENGINT_UIL_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <numbers>

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
    Color OUTLINE;
    std::string Identifier;
    bool Visible;
    UIElement() : POS({0, 0}), SIZE({0, 0}), COL({0, 0, 0, 0}) { Identifier = ""; }
private: void Setup(Color& c, Color& outline) { UIElements.push_back(this); COL = c; OUTLINE = outline; Identifier = ""; Visible = true; }
    void Setup(std::string& id, Color& c, Color& outline) { UIElements.push_back(this); COL = c; OUTLINE = outline; Identifier = id; Visible = true; }
public:
    UIElement(Vector2 p, Vector2 s, Color c, Color outline, std::string id) : POS(p), SIZE(s) { Setup(id, c, outline); }
    UIElement(float x, float y, float w, float h, Color c, Color outline, std::string id) : POS({x, y}), SIZE({w, h}) { Setup(id, c, outline); }
    UIElement(Vector4 r, Color c, Color outline, std::string id) : POS({r.x, r.y}), SIZE({r.z, r.w}) { Setup(id, c, outline); }
    UIElement(Vector2 p, Vector2 s, Color c, Color outline) : POS(p), SIZE(s) { Setup(c, outline); }
    UIElement(float x, float y, float w, float h, Color c, Color outline) : POS({x, y}), SIZE({w, h}) { Setup(c, outline); }
    UIElement(Vector4 r, Color c, Color outline) : POS({r.x, r.y}), SIZE({r.z, r.w}) { Setup(c, outline); }
    virtual void Draw() {
        DrawRectangle(POS.x, POS.y, SIZE.x, SIZE.y, COL);
        DrawRectangleLines(POS.x, POS.y, SIZE.x, SIZE.y, OUTLINE);
    }
    virtual void Unload() {}
    virtual void UpdateSize_RelativeScreenSize(float scale_x, float scale_y) {
        POS.x *= scale_x;
        POS.y *= scale_y;
        SIZE.x *= scale_x;
        SIZE.y *= scale_y;
    }
};


class UIButton;
std::vector<UIButton*> UIIButtons;
class UIButton : public UIElement {
public:
    UIButton() {}
private: void Setup() { UIIButtons.push_back(this); }
public:
    UIButton(Vector2 p, Vector2 s, Color c, Color outline, std::string id) : UIElement(p, s, c, outline, id) { Setup(); }
    UIButton(float x, float y, float w, float h, Color c, Color outline, std::string id) : UIElement(x, y, w, h, c, outline, id) { Setup(); }
    UIButton(Vector4 r, Color c, Color outline, std::string id) : UIElement(r, c, outline, id) { Setup(); }
    UIButton(Vector2 p, Vector2 s, Color c, Color outline) : UIElement(p, s, c, outline) { Setup(); }
    UIButton(float x, float y, float w, float h, Color c, Color outline) : UIElement(x, y, w, h, c, outline) { Setup(); }
    UIButton(Vector4 r, Color c, Color outline) : UIElement(r, c, outline) { Setup(); }
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
    UIImage(Vector2 p, Vector2 s, std::string fp, Color outline, std::string id) : UIElement(p, s, {0, 0, 0, 0}, outline, id) { Setup(fp); }
    UIImage(float x, float y, float w, float h, std::string fp, Color outline, std::string id) : UIElement(x, y, w, h, {0, 0, 0, 0}, outline, id) { Setup(fp); }
    UIImage(Vector4 r, std::string fp, Color outline, std::string id) : UIElement(r, {0, 0, 0, 0}, outline, id) { Setup(fp); }
    UIImage(Vector2 p, Vector2 s, std::string fp, Color outline) : UIElement(p, s, {0, 0, 0, 0}, outline) { Setup(fp); }
    UIImage(float x, float y, float w, float h, std::string fp, Color outline) : UIElement(x, y, w, h, {0, 0, 0, 0}, outline) { Setup(fp); }
    UIImage(Vector4 r, std::string fp, Color outline) : UIElement(r, {0, 0, 0, 0}, outline) { Setup(fp); }
    void Draw() override {
        DrawTexturePro(Source, {0, 0, (float) Source.width, (float) Source.height}, {POS.x, POS.y, SIZE.x, SIZE.y}, {0, 0}, 0, {255, 255, 255, 255});
        DrawRectangleLines(POS.x, POS.y, SIZE.x, SIZE.y, OUTLINE);
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
    UIImageButton(Vector2 p, Vector2 s, std::string fp, Color outline, std::string id) : UIButton(p, s, {0, 0, 0, 0}, outline, id) { Setup(fp); }
    UIImageButton(float x, float y, float w, float h, std::string fp, Color outline, std::string id) : UIButton(x, y, w, h, {0, 0, 0, 0}, outline, id) { Setup(fp); }
    UIImageButton(Vector4 r, std::string fp, Color outline, std::string id) : UIButton(r, {0, 0, 0, 0}, outline, id) { Setup(fp); }
    UIImageButton(Vector2 p, Vector2 s, std::string fp, Color outline) : UIButton(p, s, {0, 0, 0, 0}, outline) { Setup(fp); }
    UIImageButton(float x, float y, float w, float h, std::string fp, Color outline) : UIButton(x, y, w, h, {0, 0, 0, 0}, outline) { Setup(fp); }
    UIImageButton(Vector4 r, std::string fp, Color outline) : UIButton(r, {0, 0, 0, 0}, outline) { Setup(fp); }
    void Draw() override {
        DrawTexturePro(Source, {0, 0, (float) Source.width, (float) Source.height}, {POS.x, POS.y, SIZE.x, SIZE.y}, {0, 0}, 0, {255, 255, 255, 255});
        DrawRectangleLines(POS.x, POS.y, SIZE.x, SIZE.y, OUTLINE);
    }
    void Unload() override {
        UnloadTexture(Source);
    }
};

class UIText : public UIElement {
public:
    Color TextColour;
    float TextSize;
    std::string Contents;
    bool AdjustBoxWidth;
    UIText() {}
private: void Setup(Color tc, int ts, std::string& contents, bool adj_box_w) { TextColour = tc; TextSize = (float) ts; Contents = contents; AdjustBoxWidth = adj_box_w; }
public:
    UIText(Vector2 p, Vector2 s, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w, std::string id) : UIElement(p, s, c, outline, id) { Setup(tc, ts, contents, adj_box_w); }
    UIText(float x, float y, float w, float h, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w, std::string id) : UIElement(x, y, w, h, c, outline, id) { Setup(tc, ts, contents, adj_box_w); }
    UIText(Vector4 r, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w, std::string id) : UIElement(r, c, outline, id) { Setup(tc, ts, contents, adj_box_w); }
    UIText(Vector2 p, Vector2 s, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w) : UIElement(p, s, c, outline) { Setup(tc, ts, contents, adj_box_w); }
    UIText(float x, float y, float w, float h, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w) : UIElement(x, y, w, h, c, outline) { Setup(tc, ts, contents, adj_box_w); }
    UIText(Vector4 r, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w) : UIElement(r, c, outline) { Setup(tc, ts, contents, adj_box_w); }
    void Draw() override {
        DrawRectangle(POS.x, POS.y, SIZE.x, SIZE.y, COL);
        DrawRectangleLines(POS.x, POS.y, SIZE.x, SIZE.y, OUTLINE);
        Vector2 TextVector2 = MeasureTextEx(Roboto_Mono, Contents.c_str(), TextSize, 0);
        DrawTextEx(Roboto_Mono, Contents.c_str(), {POS.x+(SIZE.x-TextVector2.x)/2, POS.y+(SIZE.y-TextVector2.y)/2}, TextSize, 0, TextColour);
    }
    void UpdateSize_RelativeScreenSize(float scale_x, float scale_y) override {
        POS.x *= scale_x;
        POS.y *= scale_y;
        SIZE.x *= scale_x;
        SIZE.y *= scale_y;
        TextSize *= scale_y;
        if (AdjustBoxWidth) {
            Vector2 TextVector2 = MeasureTextEx(Roboto_Mono, Contents.c_str(), TextSize, 0);
            SIZE.x = TextVector2.x+10;
        }
    }
};
class UITextButton : public UIButton {
public:
    Color TextColour;
    float TextSize;
    std::string Contents;
    bool AdjustBoxWidth;
    UITextButton() {}
private: void Setup(Color tc, int ts, std::string& contents, bool adj_box_w) { TextColour = tc; TextSize = (float) ts; Contents = contents; AdjustBoxWidth = adj_box_w; }
public:
    UITextButton(Vector2 p, Vector2 s, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w, std::string id) : UIButton(p, s, c, outline, id) { Setup(tc, ts, contents, adj_box_w); }
    UITextButton(float x, float y, float w, float h, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w, std::string id) : UIButton(x, y, w, h, c, outline, id) { Setup(tc, ts, contents, adj_box_w); }
    UITextButton(Vector4 r, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w, std::string id) : UIButton(r, c, outline, id) { Setup(tc, ts, contents, adj_box_w); }
    UITextButton(Vector2 p, Vector2 s, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w) : UIButton(p, s, c, outline) { Setup(tc, ts, contents, adj_box_w); }
    UITextButton(float x, float y, float w, float h, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w) : UIButton(x, y, w, h, c, outline) { Setup(tc, ts, contents, adj_box_w); }
    UITextButton(Vector4 r, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w) : UIButton(r, c, outline) { Setup(tc, ts, contents, adj_box_w); }
    void Draw() override {
        DrawRectangle(POS.x, POS.y, SIZE.x, SIZE.y, COL);
        DrawRectangleLines(POS.x, POS.y, SIZE.x, SIZE.y, OUTLINE);
        Vector2 TextVector2 = MeasureTextEx(Roboto_Mono, Contents.c_str(), TextSize, 0);
        DrawTextEx(Roboto_Mono, Contents.c_str(), {POS.x+(SIZE.x-TextVector2.x)/2, POS.y+(SIZE.y-TextVector2.y)/2}, TextSize, 0, TextColour);
    }
    void UpdateSize_RelativeScreenSize(float scale_x, float scale_y) override {
        POS.x *= scale_x;
        POS.y *= scale_y;
        SIZE.x *= scale_x;
        SIZE.y *= scale_y;
        TextSize *= scale_y;
        if (AdjustBoxWidth) {
            Vector2 TextVector2 = MeasureTextEx(Roboto_Mono, Contents.c_str(), TextSize, 0);
            SIZE.x = TextVector2.x+10;
        }
    }
};

class UIElementArray;
std::vector<UIElementArray*> UIElementArrays;
class UIElementArray {
    bool VerticalStack;
    float Spacing_X;
    float Spacing_Y;
    std::vector<UIElement*> Contents;
public:
    void UpdateSpacing() {
        if (Contents.size() == 0) return;
        Vector2 Offset = Contents[0]->POS;
        for (UIElement* e : Contents) {
            e->POS = Offset;
            if (!VerticalStack) Offset.x += e->SIZE.x + Spacing_X;
            else Offset.y += e->SIZE.y + Spacing_Y;
        }
    }
    void UpdateSize_RelativeScreenSize(float scale_x, float scale_y) {
        Spacing_X *= scale_x;
        Spacing_Y *= scale_y;
        UpdateSpacing();
    }
    void ToggleVisibility() {
        for (UIElement* e : Contents) e->Visible = !e->Visible;
    }
    UIElementArray() { VerticalStack = false; Spacing_X = 0; Spacing_Y = 0; Contents = {}; UIElementArrays.push_back(this); std::cout << "Array created at memory address: " << this << std::endl; }
    UIElementArray(bool orientation, int spacing) { VerticalStack = orientation; if (VerticalStack) { Spacing_Y = (float) spacing; Spacing_X = 0; } else { Spacing_X = (float) spacing; Spacing_Y = 0; } UIElementArrays.push_back(this); std::cout << "Array created at memory address: " << this << std::endl; }
    UIElementArray(bool orientation, int spacing, std::vector<UIElement*> preset) { VerticalStack = orientation; if (VerticalStack) { Spacing_Y = (float) spacing; Spacing_X = 0; } else { Spacing_X = (float) spacing; Spacing_Y = 0; } Contents = preset; UpdateSpacing(); UIElementArrays.push_back(this); std::cout << "Array created at memory address: " << this << std::endl; }
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
        SetTextureFilter(Roboto_Mono.texture, TEXTURE_FILTER_BILINEAR);
        SetExitKey(0);
        CLEAR_BACKHROUND = true;
    }
    Window(uint16_t w, uint16_t h, const std::string &title) : WIDTH(w), HEIGHT(h) {
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(WIDTH, HEIGHT, title.c_str());
        Roboto_Mono = LoadFont("Roboto_Mono/RobotoMono-VariableFont_wght.ttf");
        SetTextureFilter(Roboto_Mono.texture, TEXTURE_FILTER_BILINEAR);
        SetExitKey(0);
        CLEAR_BACKHROUND = true;
    }
protected:
    virtual void OnRun() { }
    virtual void LateRun() { }
    virtual void OnEnd() { }
    virtual void OnUpdate_UI(float dt, float t) { }
    virtual void PreUpdate_UI(float dt, float t) { }
public:
    void Run() {
        OnRun();
        WIDTH = GetScreenWidth();
        HEIGHT = GetScreenHeight();
        for (UIElement* e : UIElements) e-> UpdateSize_RelativeScreenSize(((float) WIDTH)/2560.0f, ((float) HEIGHT)/1440.0f);
        for (UIElementArray* ea : UIElementArrays) ea->UpdateSize_RelativeScreenSize(((float) WIDTH)/2560.0f, ((float) HEIGHT)/1440.0f);
        LateRun();
        while (!WindowShouldClose()) {
            if (IsWindowResized()) {
                int newWidth = GetScreenWidth();
                int newHeight = GetScreenHeight();
                for (UIElement* e : UIElements) e->UpdateSize_RelativeScreenSize(((float) newWidth)/((float) WIDTH), ((float) newHeight)/((float) HEIGHT));
                for (UIElementArray* ea : UIElementArrays) ea->UpdateSize_RelativeScreenSize(((float) newWidth)/((float) WIDTH), ((float) newHeight)/((float) HEIGHT));
                WIDTH = newWidth;
                HEIGHT = newHeight;
            }
            float dt = GetFrameTime();
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 c_pos = GetMousePosition();
                for (UIButton* btn : UIIButtons) if (c_pos.x > btn->POS.x && c_pos.y > btn->POS.y && c_pos.x < btn->POS.x+btn->SIZE.x && c_pos.y < btn->POS.y+btn->SIZE.y && btn->Visible) { btn->OnClick(); break; }
            }
            BeginDrawing();
                PreUpdate_UI(dt, 0);
                if (CLEAR_BACKHROUND) ClearBackground(WHITE);
                for (UIElement* e : UIElements) if (e->Visible) e->Draw();
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