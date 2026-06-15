#ifndef MAIN_USERPROJECT
#define MAIN_USERPROJECT
#include "FillipSourceCodeHandler.hpp"

class GameScript : public Script {
public:
    GameScript() : Script() {}
    void OnRun() override {
        // logic
    }
    void OnUpdate(float dt) override {
        // logic
    }
};

REGISTER_GAME_CLASS(GameScript)

#endif // MAIN_USERPROJECT