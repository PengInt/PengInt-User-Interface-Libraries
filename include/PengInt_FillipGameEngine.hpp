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
    ReferenceObject* GetObject() {
        ReferenceObject* ObjectTransformed = RefMesh;
        ObjectTransformed->PLANNED_ROTATIONS.push_back(ObjTransform.Rotation);
        ObjectTransformed->X += ObjTransform.Position[0];
        ObjectTransformed->Y += ObjTransform.Position[1];
        ObjectTransformed->Z += ObjTransform.Position[2];
    }
};

class FillipGameEngineWindow : public Renderer {
private:
    Texture2D FillipLogo_WT;
    std::chrono::steady_clock::time_point start_time_chrono;
    void FillipSetup() {
        start_time_chrono = std::chrono::steady_clock::now();
        FillipLogo_WT = LoadTexture("Jeremiah-Fillip_Logo (White Text).png");
        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("A culmination of the Penguin Interactive Visual Libraries", 25, 750, 25, WHITE);
            DrawTextureEx(FillipLogo_WT, {208, 183}, 0, 6, WHITE);
        EndDrawing();
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
                DrawText("A culmination of the Penguin Interactive Visual Libraries", 25, 750, 25, WHITE);
            EndDrawing();
        }
        seconds_left = 1;
        while (!WindowShouldClose()) {
            seconds_left -= GetFrameTime();
            if (seconds_left <= 0) break;
            BeginDrawing();
                ClearBackground(BLACK);
                DrawTextureEx(FillipLogo_WT, {208, 183}, 0, 6, {255, 255, 255, (unsigned char) (255*seconds_left)});
                DrawText("A culmination of the Penguin Interactive Visual Libraries", 25, 750, 25, {255, 255, 255, (unsigned char) (255*seconds_left)});
            EndDrawing();
        }
    }
    void OnEnd() override {
        UnloadTexture(FillipLogo_WT);
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