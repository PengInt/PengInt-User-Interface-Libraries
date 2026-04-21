#ifndef PENGINT_GUIL_HPP
#define PENGINT_GUIL_HPP

#include <cstdint>

#include "PengInt_UIL.hpp"


class Renderer : public Window {
    Shader shader;
public:
    Renderer(uint16_t w, uint16_t h) : Window(w, h, "PengInt GUI") { shader = LoadShader("shaders/buffershader.glsl", "shaders/drawshader.glsl"); }
    Renderer(uint16_t w, uint16_t h, const std::string &title) : Window(w, h, title) { shader = LoadShader("shaders/buffershader.glsl", "shaders/drawshader.glsl"); }
protected:
    virtual void OnUpdate_GUI(float dt, float t) { }
    void OnUpdate_UI(float dt, float t) override {
        BeginShaderMode(shader);
            DrawRectangle(0, 0, WIDTH, HEIGHT, {255, 255, 255, 255});
        EndShaderMode();
        OnUpdate_GUI(dt, t);
    }
};


#endif //PENGINT_GUIL_HPP