#include <numbers>

#include "PengInt_GUIL.hpp"


class Game : public Renderer {
    PengIntShaderStructs::Object* obj;
public:
    Game() : Renderer(800, 800) {
        std::vector<float> vertices = {1, 1, 1, 1, 1, -1, 1, -1, 1, 1, -1, -1, -1, 1, 1, -1, 1, -1, -1, -1, 1, -1, -1, -1};
        std::vector<int> triangles = {
            0, 2, 4, 255, 0, 0, 0,
            4, 2, 6, 255, 0, 0, 0,
            1, 5, 3, 0, 255, 0, 0,
            3, 5, 7, 0, 255, 0, 0,
            0, 1, 2, 0, 0, 255, 0,
            2, 1, 3, 0, 0, 255, 0,
            4, 6, 5, 255, 255, 0, 0,
            5, 6, 7, 255, 255, 0, 0,
            0, 4, 1, 0, 255, 255, 0,
            1, 4, 5, 0, 255, 255, 0,
            2, 3, 6, 255, 0, 255, 0,
            6, 3, 7, 255, 0, 255, 0
        };
        obj = new PengIntShaderStructs::Object(0, 0, 0, vertices, triangles);
        //LoadObjectFromJSON("model_data.json");
        Run();
    }
protected:
    void OnUpdate_GUI(float dt, float t) {
        obj->PLANNED_ROTATIONS.push_back({(float) std::numbers::pi*0.5f*dt, 1, 0.5, 0});
        if (IsKeyDown(KEY_W)) {
            CameraPosition[2] += 2*dt*cos(CameraYaw);
            CameraPosition[0] -= 2*dt*sin(CameraYaw);
        }
        if (IsKeyDown(KEY_A)) {
            CameraPosition[2] -= 2*dt*sin(CameraYaw);
            CameraPosition[0] -= 2*dt*cos(CameraYaw);
        }
        if (IsKeyDown(KEY_S)) {
            CameraPosition[2] -= 2*dt*cos(CameraYaw);
            CameraPosition[0] += 2*dt*sin(CameraYaw);
        }
        if (IsKeyDown(KEY_D)) {
            CameraPosition[2] += 2*dt*sin(CameraYaw);
            CameraPosition[0] += 2*dt*cos(CameraYaw);
        }
        if (IsKeyDown(KEY_Q)) CameraPosition[1] -= 2*dt;
        if (IsKeyDown(KEY_E)) CameraPosition[1] += 2*dt;
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