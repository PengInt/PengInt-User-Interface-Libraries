#ifndef QED_UIL_HPP
#define QED_UIL_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <numbers>
#include <chrono>
#include <any>
#include <unordered_map>

#define GRAPHICS_API_OPENGL_43
#include "raylib.h"
#include "rlgl.h"
#include "external/glad.h"


Font Roboto_Mono;

namespace __UIL {
    int __debug_count = 0;
    void Debug(const std::string what) { std::cout << "UIL DEBUG [ID " << __debug_count << "]: " << what << std::endl; __debug_count++; }
}

class UIElement;
std::vector<UIElement*> UIElements;
class UIElement {
public:
    std::unordered_map<std::string, std::any> SpecialMap;
    Vector2 POS, SIZE;
    Color COL;
    Color OUTLINE;
    std::string Identifier;
    bool Visible;
    Vector4 BoundingRect;
    UIElement() : POS({0, 0}), SIZE({0, 0}), COL({0, 0, 0, 0}) { Identifier = ""; SpecialMap = {}; BoundingRect = {0, 0, 0, 0}; }
private: void Setup(Color& c, Color& outline, const std::unordered_map<std::string, std::any>& spec_map) { UIElements.push_back(this); COL = c; OUTLINE = outline; Identifier = ""; Visible = true; SpecialMap = spec_map; BoundingRect = {0, 0, 0, 0}; }
    void Setup(std::string& id, Color& c, Color& outline, const std::unordered_map<std::string, std::any>& spec_map) { UIElements.push_back(this); COL = c; OUTLINE = outline; Identifier = id; Visible = true; SpecialMap = spec_map; BoundingRect = {0, 0, 0, 0}; }
public:
    UIElement(Vector2 p, Vector2 s, Color c, Color outline, const std::unordered_map<std::string, std::any> spec_map, std::string id) : POS(p), SIZE(s) { Setup(id, c, outline, spec_map); }
    UIElement(float x, float y, float w, float h, Color c, Color outline, const std::unordered_map<std::string, std::any> spec_map, std::string id) : POS({x, y}), SIZE({w, h}) { Setup(id, c, outline, spec_map); }
    UIElement(Vector4 r, Color c, Color outline, const std::unordered_map<std::string, std::any> spec_map, std::string id) : POS({r.x, r.y}), SIZE({r.z, r.w}) { Setup(id, c, outline, spec_map); }
    UIElement(Vector2 p, Vector2 s, Color c, Color outline, const std::unordered_map<std::string, std::any> spec_map) : POS(p), SIZE(s) { Setup(c, outline, spec_map); }
    UIElement(float x, float y, float w, float h, Color c, Color outline, const std::unordered_map<std::string, std::any> spec_map) : POS({x, y}), SIZE({w, h}) { Setup(c, outline, spec_map); }
    UIElement(Vector4 r, Color c, Color outline, const std::unordered_map<std::string, std::any> spec_map) : POS({r.x, r.y}), SIZE({r.z, r.w}) { Setup(c, outline, spec_map); }
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
        BoundingRect.x *= scale_x;
        BoundingRect.y *= scale_y;
        BoundingRect.z *= scale_x;
        BoundingRect.w *= scale_y;
    }
};


class UIButton;
std::vector<UIButton*> UIButtons;
class UIButton : public UIElement {
    bool (*Callback)(UIElement*);
public:
    UIButton() {}
private: void Setup(bool (*callback)(UIElement*)) { UIButtons.push_back(this); Callback = callback; }
public:
    UIButton(Vector2 p, Vector2 s, Color c, Color outline, bool (*callback)(UIElement*), const std::unordered_map<std::string, std::any> spec_map, std::string id) : UIElement(p, s, c, outline, spec_map, id) { Setup(callback); }
    UIButton(float x, float y, float w, float h, Color c, Color outline, bool (*callback)(UIElement*), const std::unordered_map<std::string, std::any> spec_map, std::string id) : UIElement(x, y, w, h, c, outline, spec_map, id) { Setup(callback); }
    UIButton(Vector4 r, Color c, Color outline, bool (*callback)(UIElement*), const std::unordered_map<std::string, std::any> spec_map, std::string id) : UIElement(r, c, outline, spec_map, id) { Setup(callback); }
    UIButton(Vector2 p, Vector2 s, Color c, Color outline, bool (*callback)(UIElement*), const std::unordered_map<std::string, std::any> spec_map) : UIElement(p, s, c, outline, spec_map) { Setup(callback); }
    UIButton(float x, float y, float w, float h, Color c, Color outline, bool (*callback)(UIElement*), const std::unordered_map<std::string, std::any> spec_map) : UIElement(x, y, w, h, c, outline, spec_map) { Setup(callback); }
    UIButton(Vector4 r, Color c, Color outline, bool (*callback)(UIElement*), const std::unordered_map<std::string, std::any> spec_map) : UIElement(r, c, outline, spec_map) { Setup(callback); }
    bool OnClick() { return Callback(this); }
};

template <typename T>
concept IsElementWithImage = requires(T o) { o.Source; };

class UIImage : public UIElement {
public:
    Texture Source;
    UIImage() {}
private: void Setup(std::string& fp) { Source = LoadTexture(fp.c_str()); }
public:
    UIImage(Vector2 p, Vector2 s, std::string fp, Color outline, const std::unordered_map<std::string, std::any> spec_map, std::string id) : UIElement(p, s, {0, 0, 0, 0}, outline, spec_map, id) { Setup(fp); }
    UIImage(float x, float y, float w, float h, std::string fp, Color outline, const std::unordered_map<std::string, std::any> spec_map, std::string id) : UIElement(x, y, w, h, {0, 0, 0, 0}, outline, spec_map, id) { Setup(fp); }
    UIImage(Vector4 r, std::string fp, Color outline, const std::unordered_map<std::string, std::any> spec_map, std::string id) : UIElement(r, {0, 0, 0, 0}, outline, spec_map, id) { Setup(fp); }
    UIImage(Vector2 p, Vector2 s, std::string fp, Color outline, const std::unordered_map<std::string, std::any> spec_map) : UIElement(p, s, {0, 0, 0, 0}, outline, spec_map) { Setup(fp); }
    UIImage(float x, float y, float w, float h, std::string fp, Color outline, const std::unordered_map<std::string, std::any> spec_map) : UIElement(x, y, w, h, {0, 0, 0, 0}, outline, spec_map) { Setup(fp); }
    UIImage(Vector4 r, std::string fp, Color outline, const std::unordered_map<std::string, std::any> spec_map) : UIElement(r, {0, 0, 0, 0}, outline, spec_map) { Setup(fp); }
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
    UIImageButton(Vector2 p, Vector2 s, std::string fp, Color outline, bool (*callback)(UIElement*), const std::unordered_map<std::string, std::any> spec_map, std::string id) : UIButton(p, s, {0, 0, 0, 0}, outline, callback, spec_map, id) { Setup(fp); }
    UIImageButton(float x, float y, float w, float h, std::string fp, Color outline, bool (*callback)(UIElement*), const std::unordered_map<std::string, std::any> spec_map, std::string id) : UIButton(x, y, w, h, {0, 0, 0, 0}, outline, callback, spec_map, id) { Setup(fp); }
    UIImageButton(Vector4 r, std::string fp, Color outline, bool (*callback)(UIElement*), const std::unordered_map<std::string, std::any> spec_map, std::string id) : UIButton(r, {0, 0, 0, 0}, outline, callback, spec_map, id) { Setup(fp); }
    UIImageButton(Vector2 p, Vector2 s, std::string fp, Color outline, bool (*callback)(UIElement*), const std::unordered_map<std::string, std::any> spec_map) : UIButton(p, s, {0, 0, 0, 0}, outline, callback, spec_map) { Setup(fp); }
    UIImageButton(float x, float y, float w, float h, std::string fp, Color outline, bool (*callback)(UIElement*), const std::unordered_map<std::string, std::any> spec_map) : UIButton(x, y, w, h, {0, 0, 0, 0}, outline, callback, spec_map) { Setup(fp); }
    UIImageButton(Vector4 r, std::string fp, Color outline, bool (*callback)(UIElement*), const std::unordered_map<std::string, std::any> spec_map) : UIButton(r, {0, 0, 0, 0}, outline, callback, spec_map) { Setup(fp); }
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
    UIText(Vector2 p, Vector2 s, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w, const std::unordered_map<std::string, std::any> spec_map, std::string id) : UIElement(p, s, c, outline, spec_map, id) { Setup(tc, ts, contents, adj_box_w); }
    UIText(float x, float y, float w, float h, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w, const std::unordered_map<std::string, std::any> spec_map, std::string id) : UIElement(x, y, w, h, c, outline, spec_map, id) { Setup(tc, ts, contents, adj_box_w); }
    UIText(Vector4 r, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w, const std::unordered_map<std::string, std::any> spec_map, std::string id) : UIElement(r, c, outline, spec_map, id) { Setup(tc, ts, contents, adj_box_w); }
    UIText(Vector2 p, Vector2 s, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w, const std::unordered_map<std::string, std::any> spec_map) : UIElement(p, s, c, outline, spec_map) { Setup(tc, ts, contents, adj_box_w); }
    UIText(float x, float y, float w, float h, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w, const std::unordered_map<std::string, std::any> spec_map) : UIElement(x, y, w, h, c, outline, spec_map) { Setup(tc, ts, contents, adj_box_w); }
    UIText(Vector4 r, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w, const std::unordered_map<std::string, std::any> spec_map) : UIElement(r, c, outline, spec_map) { Setup(tc, ts, contents, adj_box_w); }
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
        BoundingRect.x *= scale_x;
        BoundingRect.y *= scale_y;
        BoundingRect.z *= scale_x;
        BoundingRect.w *= scale_y;
        TextSize *= scale_y;
        if (AdjustBoxWidth) {
            Vector2 TextVector2 = MeasureTextEx(Roboto_Mono, Contents.c_str(), TextSize, 0);
            SIZE.x = TextVector2.x+TextSize/3.0f;
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
    UITextButton(Vector2 p, Vector2 s, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w, bool (*callback)(UIElement*), const std::unordered_map<std::string, std::any> spec_map, std::string id) : UIButton(p, s, c, outline, callback, spec_map, id) { Setup(tc, ts, contents, adj_box_w); }
    UITextButton(float x, float y, float w, float h, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w, bool (*callback)(UIElement*), const std::unordered_map<std::string, std::any> spec_map, std::string id) : UIButton(x, y, w, h, c, outline, callback, spec_map, id) { Setup(tc, ts, contents, adj_box_w); }
    UITextButton(Vector4 r, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w, bool (*callback)(UIElement*), const std::unordered_map<std::string, std::any> spec_map, std::string id) : UIButton(r, c, outline, callback, spec_map, id) { Setup(tc, ts, contents, adj_box_w); }
    UITextButton(Vector2 p, Vector2 s, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w, bool (*callback)(UIElement*), const std::unordered_map<std::string, std::any> spec_map) : UIButton(p, s, c, outline, callback, spec_map) { Setup(tc, ts, contents, adj_box_w); }
    UITextButton(float x, float y, float w, float h, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w, bool (*callback)(UIElement*), const std::unordered_map<std::string, std::any> spec_map) : UIButton(x, y, w, h, c, outline, callback, spec_map) { Setup(tc, ts, contents, adj_box_w); }
    UITextButton(Vector4 r, Color tc, int ts, Color c, std::string contents, Color outline, bool adj_box_w, bool (*callback)(UIElement*), const std::unordered_map<std::string, std::any> spec_map) : UIButton(r, c, outline, callback, spec_map) { Setup(tc, ts, contents, adj_box_w); }
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
        BoundingRect.x *= scale_x;
        BoundingRect.y *= scale_y;
        BoundingRect.z *= scale_x;
        BoundingRect.w *= scale_y;
        TextSize *= scale_y;
        if (AdjustBoxWidth) {
            Vector2 TextVector2 = MeasureTextEx(Roboto_Mono, Contents.c_str(), TextSize, 0);
            SIZE.x = TextVector2.x+0.4f*TextSize;
        }
    }
};

class UIElementArray;
std::vector<UIElementArray*> UIElementArrays;
class UIElementArray {
public:
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
    UIElementArray() { VerticalStack = false; Spacing_X = 0; Spacing_Y = 0; Contents = {}; UIElementArrays.push_back(this); }
    UIElementArray(bool orientation, int spacing) { VerticalStack = orientation; if (VerticalStack) { Spacing_Y = (float) spacing; Spacing_X = 0; } else { Spacing_X = (float) spacing; Spacing_Y = 0; } UIElementArrays.push_back(this); }
    UIElementArray(bool orientation, int spacing, const std::vector<UIElement*> preset) { VerticalStack = orientation; if (VerticalStack) { Spacing_Y = (float) spacing; Spacing_X = 0; } else { Spacing_X = (float) spacing; Spacing_Y = 0; } Contents = preset; UpdateSpacing(); UIElementArrays.push_back(this); }
    void push_back(UIElement* ui_element) { Contents.push_back(ui_element); UpdateSpacing(); }
    UIElement* operator[](size_t i) { return Contents[i]; }
    const UIElement* operator[](size_t i) const { return Contents[i]; }
    int size() { return Contents.size(); }
};
class UIElementArrayCentered : public UIElementArray {
    Vector2 Center;
    UIElementArrayCentered(bool orientation, int spacing, const Vector2 center) : UIElementArray(orientation, spacing) { Center = center; }
    UIElementArrayCentered(bool orientation, int spacing, const std::vector<UIElement*> preset, const Vector2 center) : UIElementArray(orientation, spacing, preset) { Center = center; }
    void UpdateCenter() {
        if (size() == 0) return;
        UpdateSpacing();
        Vector2 totalSize;
        if (VerticalStack) {
            totalSize = {0, Contents[size()-1]->POS.y+Contents[size()-1]->SIZE.y-Contents[0]->POS.y};
            float min = 0;
            for (UIElement* e : Contents) if (e->POS.x+e->SIZE.x>totalSize.x) totalSize.x = e->POS.x+e->SIZE.x; else if (e->POS.x < min) min = e->POS.x;
            totalSize.x -= min;
        } else {
            totalSize = {Contents[size()-1]->POS.x+Contents[size()-1]->SIZE.x-Contents[0]->POS.x, 0};
            float min = 0;
            for (UIElement* e : Contents) if (e->POS.y+e->SIZE.y>totalSize.y) totalSize.y = e->POS.y+e->SIZE.y; else if (e->POS.y < min) min = e->POS.y;
            totalSize.y -= min;
        }
        Contents[0]->POS = {Center.x - totalSize.x/2, Center.y - totalSize.y/2};
        UpdateSpacing();
    }
};
class UIElementArrayScrolling;
std::vector<UIElementArrayScrolling*> UIElementArrayScrollings;
class UIElementArrayScrolling : public UIElementArray {
    float Offset;
public:
    UIElement* BoundingBox;
    void UpdateBoundingBox() const { for (UIElement* e : Contents) e->BoundingRect = {BoundingBox->POS.x, BoundingBox->POS.y, BoundingBox->SIZE.x, BoundingBox->SIZE.y}; }
    UIElementArrayScrolling(bool orientation, int spacing, UIElement* bg) : UIElementArray(orientation, spacing) { BoundingBox = bg; Offset = 0; UpdateBoundingBox(); UIElementArrayScrollings.push_back(this); }
    UIElementArrayScrolling(bool orientation, int spacing, const std::vector<UIElement*> preset, UIElement* bg) : UIElementArray(orientation, spacing, preset) { BoundingBox = bg; Offset = 0; UpdateBoundingBox(); UIElementArrayScrollings.push_back(this); }
    void OnScroll(float amount) {
        float true_amount = amount;
        Offset += amount;
        if (Offset > 0) { true_amount -= Offset; Offset = 0; }
        if (VerticalStack) for (UIElement* e : Contents) e->POS.y += true_amount;
        else for (UIElement* e : Contents) e->POS.x += true_amount;
    }
    void push_back(UIElement* ui_element) { Contents.push_back(ui_element); UpdateSpacing(); UpdateBoundingBox(); }
};


void SetUIScale_Selective(const std::vector<UIElement*> elements) {
    for (UIElement* e : elements) e->UpdateSize_RelativeScreenSize(((float) GetScreenWidth())/800.0f, ((float) GetScreenHeight())/800.0f);
}

class Window {
public:
    uint16_t WIDTH, HEIGHT;
    bool CLEAR_BACKHROUND;
    bool EXIT_TO_MENU = true;
    bool IS_UI_ALREADY_SCALED = false;
    bool VIRTUAL_CURSOR = false;
    Vector2 VCURSOR_POS = {0, 0};
    Window(uint16_t w, uint16_t h) : WIDTH(w), HEIGHT(h) {
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(WIDTH, HEIGHT, "QED UI");
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
    virtual void LateUpdate_UI(float dt, float t) {}
    bool UI_LOOP(float dt, float t) {
        EXIT_TO_MENU = false;
        if (IsKeyPressed(KEY_GRAVE)) if (IsWindowMaximized()) RestoreWindow(); else MaximizeWindow();
        if (IsWindowResized()) {
            int newWidth = GetScreenWidth();
            int newHeight = GetScreenHeight();
            for (UIElement* e : UIElements) e->UpdateSize_RelativeScreenSize(((float) newWidth)/((float) WIDTH), ((float) newHeight)/((float) HEIGHT));
            for (UIElementArray* ea : UIElementArrays) ea->UpdateSize_RelativeScreenSize(((float) newWidth)/((float) WIDTH), ((float) newHeight)/((float) HEIGHT));
            WIDTH = newWidth;
            HEIGHT = newHeight;
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_ENTER)) {
            Vector2 c_pos = GetMousePosition();
            if (VIRTUAL_CURSOR) c_pos = VCURSOR_POS;
            for (UIButton* btn : UIButtons) if (c_pos.x > btn->POS.x && c_pos.y > btn->POS.y && c_pos.x < btn->POS.x+btn->SIZE.x && c_pos.y < btn->POS.y+btn->SIZE.y && btn->Visible) { if (btn->OnClick()) return true; break; }
        }
        if (GetMouseWheelMove() != 0) {
            Vector2 c_pos = GetMousePosition();
            float amt = GetMouseWheelMove()*10;
            if (VIRTUAL_CURSOR) { c_pos = VCURSOR_POS; if (IsKeyDown(KEY_EQUAL)) amt = 10; else if (IsKeyDown(KEY_MINUS)) amt = -10; }
            for (UIElementArrayScrolling* eas : UIElementArrayScrollings) if (c_pos.x > eas->BoundingBox->POS.x && c_pos.y > eas->BoundingBox->POS.y && c_pos.x < eas->BoundingBox->POS.x+eas->BoundingBox->SIZE.x && c_pos.y < eas->BoundingBox->POS.y+eas->BoundingBox->SIZE.y && eas->BoundingBox->Visible) eas->OnScroll(amt);
        }
        if (IsKeyPressed(KEY_RIGHT_SHIFT)) { VIRTUAL_CURSOR = !VIRTUAL_CURSOR; __UIL::Debug("VCUR Toggled"); }
        if (VIRTUAL_CURSOR) {
            if (IsKeyDown(KEY_LEFT)) VCURSOR_POS.x -= dt*25.0f;
            if (IsKeyDown(KEY_RIGHT)) VCURSOR_POS.x += dt*25.0f;
            if (IsKeyDown(KEY_UP)) VCURSOR_POS.y -= dt*25.0f;
            if (IsKeyDown(KEY_DOWN)) VCURSOR_POS.y += dt*25.0f;
        }
        LateUpdate_UI(dt, t);
        BeginDrawing();
        if (CLEAR_BACKHROUND) ClearBackground(BLACK);
        for (UIElement* e : UIElements) if (e->Visible) {
            if (e->BoundingRect.z > 0 && e->BoundingRect.w > 0) BeginScissorMode(e->BoundingRect.x, e->BoundingRect.y, e->BoundingRect.z, e->BoundingRect.w);
            e->Draw();
            if (e->BoundingRect.z > 0 && e->BoundingRect.w > 0) EndScissorMode();
        }
        if (VIRTUAL_CURSOR) DrawRectangle(VCURSOR_POS.x-2, VCURSOR_POS.y-2, 4, 4, {255, 127, 0, 255});
        EndDrawing();
        return false;
    }
    void RESET_UI() {
        for (UIElement* e : UIElements) delete e;
        UIElements.clear();
        for (UIElementArray* ea : UIElementArrays) delete ea;
        UIElementArrays.clear();
        UIElementArrayScrollings.clear();
        UIButtons.clear();
    }
    void SetUIScale() const {
        for (UIElement* e : UIElements) e->UpdateSize_RelativeScreenSize(((float) WIDTH)/800.0f, ((float) HEIGHT)/800.0f);
        for (UIElementArray* ea : UIElementArrays) ea->UpdateSize_RelativeScreenSize(((float) WIDTH)/800.0f, ((float) HEIGHT)/800.0f);
    }
    void SetUIScale(const std::vector<int> element_indices, const std::vector<int> elementArray_indices) {
        for (int i : element_indices) UIElements[i]->UpdateSize_RelativeScreenSize(((float) WIDTH)/800.0f, ((float) HEIGHT)/800.0f);
        for (int i : elementArray_indices) UIElementArrays[i]->UpdateSize_RelativeScreenSize(((float) WIDTH)/800.0f, ((float) HEIGHT)/800.0f);
    }
    void SetUIScale(const std::vector<UIElement*> elements) {
        for (UIElement* e : elements) e->UpdateSize_RelativeScreenSize(((float) WIDTH)/800.0f, ((float) HEIGHT)/800.0f);
    }
    void SetUIScale(const std::vector<UIElementArray*> elementArrays) {
        for (UIElementArray* ea : elementArrays) ea->UpdateSize_RelativeScreenSize(((float) WIDTH)/800.0f, ((float) HEIGHT)/800.0f);
    }
public:
    void Run() {
        /**/#if defined(_WIN32)
            std::system("cls");
        #else
            std::system("clear");
        #endif/**/
        OnRun();
        WIDTH = GetScreenWidth();
        HEIGHT = GetScreenHeight();
        if (!IS_UI_ALREADY_SCALED) SetUIScale();
        LateRun();
        while (EXIT_TO_MENU && !WindowShouldClose()) {
            while (!WindowShouldClose()) {
                float dt = GetFrameTime();
                PreUpdate_UI(dt, 0);
                if (UI_LOOP(dt, 0)) { EXIT_TO_MENU = true; break; }
                OnUpdate_UI(dt, 0);
            }
            OnEnd();
        }
        for (UIElement* e : UIElements) e->Unload();
        UnloadFont(Roboto_Mono);
        CloseWindow();
    }
};

#endif //QED_UIL_HPP