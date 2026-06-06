#include <numbers>
#include <iostream>

#include "PengInt_FillipGameEngine.hpp"


class Game : public FillipGameEngineWindow {
    PengIntShaderStructs::Object* obj;
public:
    Game() : FillipGameEngineWindow() {
        obj = LoadObjectFromJSON("model_data.json");
        Run();
    }
protected:
    void OnUpdate_GUI(float dt, float t) {
        obj->PLANNED_ROTATIONS.push_back({(float) std::numbers::pi*0.5f*dt, 1, 1, 1});
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
        if (IsKeyDown(KEY_UP)) CameraPitch += (float) std::numbers::pi*0.5f*dt;
        if (IsKeyDown(KEY_DOWN)) CameraPitch -= (float) std::numbers::pi*0.5f*dt;
        if (IsKeyDown(KEY_LEFT)) CameraYaw += (float) std::numbers::pi*0.5f*dt;
        if (IsKeyDown(KEY_RIGHT)) CameraYaw -= (float) std::numbers::pi*0.5f*dt;
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