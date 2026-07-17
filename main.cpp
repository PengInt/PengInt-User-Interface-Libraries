#include <numbers>
#include <iostream>

#include "QED_FGE.hpp"


class Game : public FillipGameEngineWindow {
    ShaderStructs::Object* cube;
    ShaderStructs::Object* cubed;
public:
    Game() : FillipGameEngineWindow() {
        Run();
    }
protected:
    void Fillip_OnUpdate(float dt, float t) override {
        
    }
};

int main() {
    Game game = Game();
    return 0;
}