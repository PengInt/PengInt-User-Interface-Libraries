#include "PengInt_R-GUIL.hpp"
#include "Workspace/Code/main.hpp"

#include <iostream>

class GameRenderer : public Renderer {
public:
    GameRenderer() : Renderer(800, 800, "UserProject") {}
    void OnRun() override {
        UserGameInstance->OnRun();
    }
    void OnUpdate_GUI(float dt, float t) override {
        UserGameInstance->OnUpdate(dt);
    }
};

int main() {
    std::cout << "Loading Application" << std::endl;
    GameRenderer GameWindow = GameRenderer();
    GameWindow.Run();
    return 0;
}