#include <numbers>

#include "PengInt_GUIL.hpp"


class Game : public Renderer {
    PengIntShaderStructs::Object* obj;
public:
    Game() : Renderer(800, 800) {
        std::vector<float> vertices = {1, 1, 1, 1, 1, -1, 1, -1, 1, 1, -1, -1, -1, 1, 1, -1, 1, -1, -1, -1, 1, -1, -1, -1};
        std::vector<int> triangles = {
            0, 1, 2, 255, 0, 0, 0,
            1, 2, 3, 0, 255, 0, 0,
            2, 3, 4, 0, 0, 255, 0,
            3, 4, 5, 255, 255, 0, 0,
            4, 5, 6, 0, 255, 255, 0,
            5, 6, 7, 255, 0, 255, 0,
            6, 7, 0, 127, 0, 255, 0,
            7, 0, 1, 255, 0, 127, 0
        };
        obj = new PengIntShaderStructs::Object(0, 0, 0, vertices, triangles);
        Run();
    }
protected:
    void OnUpdate_GUI(float dt, float t) {
        obj->PLANNED_ROTATIONS.push_back({(float) std::numbers::pi*dt, 1, 0, 0});
    }
};

int main() {
    Game game = Game();
    return 0;
}