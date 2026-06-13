#ifndef PENGINT_FILLIP_HPP
#define PENGINT_FILLIP_HPP


#include <chrono>

#include "PengInt_R-GUIL.hpp"


class ObjectTransform {
public:
    std::array<float, 4> Rotation;
    std::array<float, 3> Position;
    ObjectTransform() : Rotation({0,0,0,0}), Position({0,0,0}) {}
};

typedef PengIntShaderStructs::Object ReferenceObject;
class MeshObject {
    ReferenceObject* RefMesh;
    ObjectTransform ObjTransform;
public:
    MeshObject(ReferenceObject* refobj) : RefMesh(refobj), ObjTransform(ObjectTransform()) { }
};

class FillipGameEngineWindow : public Renderer {
    Texture FillipLogo_WT;
    std::chrono::steady_clock::time_point start_time_chrono;
    UIElementArray TopBarArray;
    void FillipSetup() {
        start_time_chrono = std::chrono::steady_clock::now();
        FillipLogo_WT = LoadTexture("Jeremiah-Fillip_Logo (White Text).png");
        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("A culmination of the Penguin Interactive Visual Libraries", 25, 750, 25, WHITE);
            DrawTextureEx(FillipLogo_WT, {208, 183}, 0, 6, WHITE);
        EndDrawing();
        TopBarArray = UIElementArray(false, 5, {
            new UITextButton({10, 10, 75, 35}, {255, 255, 255, 255}, 25, {0, 0, 0, 255}, "File", "File Button (Top Bar)"),
            new UITextButton({0, 0, 75, 35}, {255, 255, 255, 255}, 25, {0, 0, 0, 255}, "Edit", "Edit Button (Top Bar)"),
            new UITextButton({0, 0, 195, 35}, {255, 255, 255, 255}, 25, {0, 0, 0, 255}, "Version Control", "VCS Button (Top Bar)")
        });
    }
    virtual void Fillip_OnRun() {}
    virtual void Fillip_OnUpdate(float dt, float t) {}
    void OnUpdate_GUI(float dt, float t) override {
        Vector2 MouseM = GetMouseDelta();
        float mvtmult = 1;
        if (IsKeyDown(KEY_LEFT_SHIFT)) mvtmult = 2;
        if (IsKeyDown(KEY_W)) {
            CameraPosition[2] += mvtmult*2*dt*cos(CameraYaw);
            CameraPosition[0] -= mvtmult*2*dt*sin(CameraYaw);
        }
        if (IsKeyDown(KEY_A)) {
            CameraPosition[2] -= mvtmult*2*dt*sin(CameraYaw);
            CameraPosition[0] -= mvtmult*2*dt*cos(CameraYaw);
        }
        if (IsKeyDown(KEY_S)) {
            CameraPosition[2] -= mvtmult*2*dt*cos(CameraYaw);
            CameraPosition[0] += mvtmult*2*dt*sin(CameraYaw);
        }
        if (IsKeyDown(KEY_D)) {
            CameraPosition[2] += mvtmult*2*dt*sin(CameraYaw);
            CameraPosition[0] += mvtmult*2*dt*cos(CameraYaw);
        }
        if (IsKeyDown(KEY_Q)) CameraPosition[1] -= mvtmult*2*dt;
        if (IsKeyDown(KEY_E)) CameraPosition[1] += mvtmult*2*dt;
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (IsCursorHidden()) EnableCursor();
            else DisableCursor();
        }
        if (IsCursorHidden()) CameraYaw -= MouseM.x*std::numbers::pi/512;
        if (IsCursorHidden()) CameraPitch -= MouseM.y*std::numbers::pi/512;
        if (CameraPitch > std::numbers::pi*0.5f) CameraPitch = std::numbers::pi*0.5f;
        else if (CameraPitch < -std::numbers::pi*0.5f) CameraPitch = -std::numbers::pi*0.5f;
        if (CameraYaw > std::numbers::pi) CameraYaw -= 2*std::numbers::pi;
        else if (CameraYaw < -std::numbers::pi) CameraYaw += 2*std::numbers::pi;
        Fillip_OnUpdate(dt, t);
    }
    void OnRun() override {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        float seconds_left = 1 - std::chrono::duration_cast<std::chrono::seconds>(now-start_time_chrono).count();
        while (!WindowShouldClose()) {
            seconds_left -= GetFrameTime();
            if (seconds_left <= 0) break;
            BeginDrawing();
                ClearBackground(BLACK);
                DrawTextureEx(FillipLogo_WT, {208, 183}, 0, 6, WHITE);
                DrawTextEx(Roboto_Mono, "A culmination of the Penguin Interactive Visual Libraries", {10, 760}, 30, 0, WHITE);
            EndDrawing();
        }
        seconds_left = 1;
        while (!WindowShouldClose()) {
            seconds_left -= GetFrameTime();
            if (seconds_left <= 0) break;
            BeginDrawing();
                ClearBackground(BLACK);
                DrawTextureEx(FillipLogo_WT, {208, 183}, 0, 6, {255, 255, 255, (unsigned char) (255*seconds_left)});
                DrawTextEx(Roboto_Mono, "A culmination of the Penguin Interactive Visual Libraries", {10, 760}, 30, 0, {255, 255, 255, (unsigned char) (255*seconds_left)});
            EndDrawing();
        }
        Fillip_OnRun();
    }
public:
    FillipGameEngineWindow() : Renderer(800, 800, "Fillip Game Engine") {
        FillipSetup();
    }
    FillipGameEngineWindow(std::string GameName) : Renderer(800, 800, GameName) {
        FillipSetup();
    }
};

#endif //PENGINT_FILLIP_HPP