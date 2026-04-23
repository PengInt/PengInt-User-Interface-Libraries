#ifndef PENGINT_GUIL_HPP
#define PENGINT_GUIL_HPP

#include <cstdint>

#include "PengInt_UIL.hpp"

#define GRAPHICS_API_OPENGL_43
#include "rlgl.h"


class Renderer : public Window {
    Shader shader;
    unsigned int ShaderID;
    unsigned int ProgramID;
    unsigned int ssbo;
private:
    void LoadComputeShader() {
        char* code = LoadFileText("shaders/buffershader.glsl");
        ShaderID = rlLoadShader(code, RL_COMPUTE_SHADER);
        ProgramID = rlLoadShaderProgramCompute(ShaderID);
        UnloadFileText(code);

        std::vector<float> myData = { 10.0f, 20.0f, 30.0f, 40.0f };
        ssbo = rlLoadShaderBuffer(myData.size() * sizeof(float), myData.data(), RL_DYNAMIC_COPY);
    }
public:
    Renderer(uint16_t w, uint16_t h) : Window(w, h, "PengInt GUI") {
        shader = LoadShader(0, "shaders/drawshader.glsl");
    }
    Renderer(uint16_t w, uint16_t h, const std::string &title) : Window(w, h, title) {
        shader = LoadShader(0, "shaders/drawshader.glsl");
    }
protected:
    void OnRun() {}
    virtual void OnUpdate_GUI(float dt, float t) { }
    void OnUpdate_UI(float dt, float t) override {
        BeginShaderMode(shader);
            DrawRectangle(0, 0, WIDTH, HEIGHT, {255, 255, 255, 255});
        EndShaderMode();
        OnUpdate_GUI(dt, t);
    }
};


#endif //PENGINT_GUIL_HPP