#ifndef PENGINT_GUIL_HPP
#define PENGINT_GUIL_HPP

#include <cstdint>
#include <array>

#include "PengInt_UIL.hpp"

#define GRAPHICS_API_OPENGL_43
#include "rlgl.h"


float VectorDotProduct(const std::array<float, 3>& v1, const std::array<float, 3>& v2) {
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

std::array<float, 3> VectorCrossProduct(const std::array<float, 3>& v1, const std::array<float, 3>& v2) {
    return {
        v1[1]*v2[2] - v1[2]*v2[1],
        -v1[0]*v2[2] + v1[2]*v2[0],
        v1[0]*v2[1] - v1[1]*v2[0]
    };
}

std::array<float, 4> QuaternionMultiplication(const std::array<float, 4>& q1, const std::array<float, 4>& q2) {
    return {
        q1[0]*q2[0] - q1[1]*q2[1] - q1[2]*q2[2] - q1[3]*q2[3],
        q1[0]*q2[1] + q1[1]*q2[0] - q1[2]*q2[3] + q1[3]*q2[2],
        q1[0]*q2[2] + q1[1]*q2[3] + q1[2]*q2[0] - q1[3]*q2[1],
        q1[0]*q2[3] - q1[1]*q2[2] + q1[2]*q2[1] + q1[3]*q2[0]
    };
}

namespace PengIntShaderStructs {
    struct Vertex {
        float px, py, pz;
        float cx, cy, cz;
        float w, x, y, z;
        int oi, vi;
        float padding[2];
    };
    struct Triangle {
        float x1, y1, z1, _pad1;
        float x2, y2, z2, _pad2;
        float x3, y3, z3, _pad3;
        float r, g, b, _pad4;
        float cx, cy, cz, _pad5;
        float w, x, y, z;
    };
    class Object {
        float X, Y, Z;
        std::vector<float> Verticies;
        std::vector<int> Triangles;
    private:
        std::vector<std::array<float, 4>> PLANNED_ROTATIONS;
    public:
        Object(float x, float y, float z, std::vector<float>& v, std::vector<int>& t) : X(x), Y(y), Z(z), Verticies(v), Triangles(t) { }
        std::vector<Vertex> GetVertexData(int oi, int vi) {
            std::vector<Vertex> output;
            std::array<float, 4> quaternion = PLANNED_ROTATIONS[0];
            for (int i = 1; i < PLANNED_ROTATIONS.size(); i++) QuaternionMultiplication(quaternion, PLANNED_ROTATIONS[i]);
            for (int i = 0; i < Verticies.size(); i += 3) output.push_back({
                Verticies[i], Verticies[i+1], Verticies[i+2],
                X, Y, Z,
                quaternion[0], quaternion[1], quaternion[2], quaternion[3],
                oi, vi,
                {0, 0}
            });
            return output;
        }
        std::vector<Triangle> GetTriangleData(float cx, float cy, float cz, float w, float x, float y, float z) {
            std::vector<Triangle> output;
            for (int i = 0; i < Triangles.size(); i += 6) output.push_back({
                Verticies[3*Triangles[i]], Verticies[3*Triangles[i]+1], Verticies[3*Triangles[i]+2], 0,
                Verticies[3*Triangles[i+1]], Verticies[3*Triangles[i+1]+1], Verticies[3*Triangles[i+1]+2], 0,
                Verticies[3*Triangles[i+2]], Verticies[3*Triangles[i+2]+1], Verticies[3*Triangles[i+2]+1], 0,
                (float) Triangles[i+3], (float) Triangles[i+4], (float) Triangles[i+5], 0,
                cx, cy, cz, 0,
                w, x, y, z
            });
        }
    };
}

class Renderer : public Window {
private:
    Shader DrawShader;
    unsigned int RotateProgram;
    unsigned int BufferProgram;
    unsigned int vertexSSBO;
    unsigned int triangleSSBO;
    unsigned int zBufferSSBO;
    int totalVertices = 0;
    int totalTriangles = 0;
    void LoadShaders() {
        DrawShader = LoadShader(0, "shaders/drawshader.glsl");

        char* rotCode = LoadFileText("shaders/rotateshader.glsl");
        unsigned int rotShader = rlLoadShader(rotCode, RL_COMPUTE_SHADER);
        RotateProgram = rlLoadShaderProgramCompute(rotShader);
        UnloadFileText(rotCode);

        char* bufCode = LoadFileText("shaders/buffershader.glsl");
        unsigned int bufShader = rlLoadShader(bufCode, RL_COMPUTE_SHADER);
        RotateProgram = rlLoadShaderProgramCompute(bufShader);
        UnloadFileText(bufCode);
    }
public:
    Renderer(uint16_t w, uint16_t h) : Window(w, h, "PengInt GUI") {
        LoadShaders();
    }
    Renderer(uint16_t w, uint16_t h, const std::string &title) : Window(w, h, title) {
        LoadShaders();
    }
protected:
    void OnRun() {}
    virtual void OnUpdate_GUI(float dt, float t) { }
    void PreUpdate_UI(float dt, float t) override {
        BeginShaderMode(DrawShader);
            DrawRectangle(0, 0, WIDTH, HEIGHT, {255, 255, 255, 255});
        EndShaderMode();
        OnUpdate_GUI(dt, t);
    }
};


#endif //PENGINT_GUIL_HPP