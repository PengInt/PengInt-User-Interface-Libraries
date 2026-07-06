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
        /*cube = LoadObjectFromJSON("cube.json");
        cubed = LoadObjectFromJSON("cube.json");
        cube->Y = 0.5;
        cubed->Y = 0.5;
        cubed->X = 1.5;
        cubed->Y = 1.0;
        ground = LoadObjectFromJSON("ground.json");
        mat_cubes = LoadMaterialFromJSON("cube material.json");
        mat_ground = LoadMaterialFromJSON("ground material.json");
        ls1 = LoadLightSourceFromJSON("lightsource 1.json");*/
        Run();
    }
protected:
    void Fillip_OnUpdate(float dt, float t) override {
        // stuff here
    }
};

int main() {
    Game game = Game();
    return 0;
}