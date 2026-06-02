#ifndef PENGINT_FILLIP_HPP
#define PENGINT_FILLIP_HPP

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
    void FillipSetup() {
        FillipLogo_WT = LoadTexture("Jeremiah-Fillip_Logo (White Text).png");
        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("A culmination of the Penguin Interactive Visual Libraries", 25, 750, 25, WHITE);
            DrawTextureEx(FillipLogo_WT, {208, 183}, 0, 6, WHITE);
        EndDrawing();
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