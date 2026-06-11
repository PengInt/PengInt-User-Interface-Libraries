#include <numbers>
#include <iostream>

#include "PengInt_FGE.hpp"


class Game : public FillipGameEngineWindow {
    PengIntShaderStructs::Object* cube;
    PengIntShaderStructs::Object* cubed;
    PengIntShaderStructs::Object* ground;
    PengIntShaderStructs::Material* mat_cubes;
    PengIntShaderStructs::Material* mat_ground;
    PengIntShaderStructs::LightSource* ls1;
public:
    Game() : FillipGameEngineWindow() {
        cube = LoadObjectFromJSON("cube.json");
        cubed = LoadObjectFromJSON("cube.json");
        cube->Y = 0.5;
        cubed->Y = 0.5;
        cubed->X = 1.5;
        cubed->Y = 1.0;
        ground = LoadObjectFromJSON("ground.json");
        mat_cubes = LoadMaterialFromJSON("cube material.json");
        mat_ground = LoadMaterialFromJSON("ground material.json");
        ls1 = LoadLightSourceFromJSON("lightsource 1.json");
        Run();
    }
protected:
    void Fillip_OnRun() override {
        DisableCursor();
    }
    void Fillip_OnUpdate(float dt, float t) override {
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
    }
};

int main() {
    Game game = Game();
    return 0;
}