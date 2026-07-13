#ifndef FILLIP_SOURCE_CODE_HANDLER
#define FILLIP_SOURCE_CODE_HANDLER
#include <array>

#include "QED_R-GUIL.hpp"

std::array<float, 2> GetMouseMovement() {
    return {GetMouseDelta().x, GetMouseDelta().y};
}

class Script {
    std::chrono::steady_clock::time_point start_time_chrono;
    Texture FillipLogo_WT;
public:
    std::array<float, 3> CameraPosition;
    float CameraPitch, CameraYaw, CameraRoll;
    void SRC_Init() {
        start_time_chrono = std::chrono::steady_clock::now();
        FillipLogo_WT = LoadTexture("images/Jeremiah-Fillip_Logo (White Text).png");
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("A culmination of the QED Visual Libraries", 25, 750, 25, WHITE);
        DrawTextureEx(FillipLogo_WT, {208, 183}, 0, 6, WHITE);
        EndDrawing();
    }
    Script() {
        CameraPosition = {0, 0, 0};
    }
    void SRC_OnRun() {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        float seconds_left = 1 - std::chrono::duration_cast<std::chrono::seconds>(now-start_time_chrono).count();
        while (!WindowShouldClose()) {
            seconds_left -= GetFrameTime();
            if (seconds_left <= 0) break;
            BeginDrawing();
            ClearBackground(BLACK);
            DrawTextureEx(FillipLogo_WT, {208, 183}, 0, 6, WHITE);
            DrawTextEx(Roboto_Mono, "A culmination of the QED Visual Libraries", {10, 760}, 30, 0, WHITE);
            EndDrawing();
        }
        seconds_left = 1;
        while (!WindowShouldClose()) {
            seconds_left -= GetFrameTime();
            if (seconds_left <= 0) break;
            BeginDrawing();
            ClearBackground(BLACK);
            DrawTextureEx(FillipLogo_WT, {208, 183}, 0, 6, {255, 255, 255, (unsigned char) (255*seconds_left)});
            DrawTextEx(Roboto_Mono, "A culmination of the QED Visual Libraries", {10, 760}, 30, 0, {255, 255, 255, (unsigned char) (255*seconds_left)});
            EndDrawing();
        }
        OnRun();
    }
    virtual void OnRun() {}
    virtual void OnUpdate(float dt) {}
};

extern Script* UserGameInstance;
#define REGISTER_GAME_CLASS(ClassName) \
    Script* UserGameInstance = new ClassName();

#endif // FILLIP_SOURCE_CODE_HANDLER