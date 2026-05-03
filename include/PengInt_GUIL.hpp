#ifndef PENGINT_GUIL_HPP
#define PENGINT_GUIL_HPP

#include <cstdint>
#include <array>
#include <cmath>

#include "PengInt_UIL.hpp"

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

std::array<float, 4> fromAngleAxis(std::array<float, 4> q) {
    float angle = q[0], x = q[1], y = q[2], z = q[3];
    float halfAngle = angle*0.5;
    float s = std::sin(halfAngle);
    return {std::cos(halfAngle), x * s, y * s, z * s};
}

std::array<float, 4> QuaternionMultiplication(const std::array<float, 4>& q1, const std::array<float, 4>& q2) {
    return {
        q1[0]*q2[0] - q1[1]*q2[1] - q1[2]*q2[2] - q1[3]*q2[3],
        q1[0]*q2[1] + q1[1]*q2[0] + q1[2]*q2[3] - q1[3]*q2[2],
        q1[0]*q2[2] - q1[1]*q2[3] + q1[2]*q2[0] + q1[3]*q2[1],
        q1[0]*q2[3] + q1[1]*q2[2] - q1[2]*q2[1] + q1[3]*q2[0]
    };
}

std::array<float, 4> toAngleAxis(const std::array<float, 4>& q) {
    float angle = 2*std::acos(q[0]);
    float s = std::sqrt(1-q[0]*q[0]);
    if (s < 0.001) return {angle, 1, 0, 0};
    return {angle, q[1] / s, q[2] / s, q[3] / s};
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
    class Object;
    class Object {
    public:
        static std::vector<Object*> OBJECTS;
        float X, Y, Z;
        std::vector<float> Verticies;
        std::vector<int> Triangles;
    private:
        std::vector<std::array<float, 4>> PLANNED_ROTATIONS;
    public:
        Object(float x, float y, float z, std::vector<float>& v, std::vector<int>& t) : X(x), Y(y), Z(z), Verticies(v), Triangles(t) { OBJECTS.push_back(this); }
        std::vector<Vertex> GetVertexData(int oi, int vi) {
            std::vector<Vertex> output;
            std::array<float, 4> quaternion = {1, 0, 0, 0};
            for (int i = 0; i < PLANNED_ROTATIONS.size(); i++) quaternion = toAngleAxis(QuaternionMultiplication(fromAngleAxis(quaternion), fromAngleAxis(PLANNED_ROTATIONS[i])));
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
            for (int i = 0; i < Triangles.size(); i += 7) output.push_back({
                Verticies[3*Triangles[i]], Verticies[3*Triangles[i]+1], Verticies[3*Triangles[i]+2], 0,
                Verticies[3*Triangles[i+1]], Verticies[3*Triangles[i+1]+1], Verticies[3*Triangles[i+1]+2], 0,
                Verticies[3*Triangles[i+2]], Verticies[3*Triangles[i+2]+1], Verticies[3*Triangles[i+2]+2], 0,
                (float) Triangles[i+3], (float) Triangles[i+4], (float) Triangles[i+5], 0,
                cx, cy, cz, 0,
                w, x, y, z
            });
            return output;
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
        BufferProgram = rlLoadShaderProgramCompute(bufShader);
        UnloadFileText(bufCode);

        vertexSSBO = 0;
        triangleSSBO = 0;
        zBufferSSBO = 0;
    }
    void SyncGPUData(const std::vector<PengIntShaderStructs::Vertex>& verticies, const std::vector<PengIntShaderStructs::Triangle>& triangles) {
        if (vertexSSBO == 0) {
            vertexSSBO = rlLoadShaderBuffer(verticies.size() * sizeof(PengIntShaderStructs::Vertex), verticies.data(), RL_DYNAMIC_COPY);
        } else if (totalVertices < verticies.size()) {
            rlUnloadShaderBuffer(vertexSSBO);
            vertexSSBO = rlLoadShaderBuffer(verticies.size() * sizeof(PengIntShaderStructs::Vertex), verticies.data(), RL_DYNAMIC_COPY);
        } else {
            rlUpdateShaderBuffer(vertexSSBO, verticies.data(), verticies.size() * sizeof(PengIntShaderStructs::Vertex), 0);
        }
        totalVertices = verticies.size();

        if (triangleSSBO == 0) {
            triangleSSBO = rlLoadShaderBuffer(triangles.size() * sizeof(PengIntShaderStructs::Triangle), triangles.data(), RL_DYNAMIC_COPY);
        } else if (totalTriangles < triangles.size()) {
            rlUnloadShaderBuffer(triangleSSBO);
            triangleSSBO = rlLoadShaderBuffer(triangles.size() * sizeof(PengIntShaderStructs::Triangle), triangles.data(), RL_DYNAMIC_COPY);
        } else {
            rlUpdateShaderBuffer(triangleSSBO, triangles.data(), triangles.size() * sizeof(PengIntShaderStructs::Triangle), 0);
        }
        totalTriangles = triangles.size();

        if (zBufferSSBO == 0) {
            zBufferSSBO = rlLoadShaderBuffer(WIDTH*HEIGHT*sizeof(float), NULL, RL_DYNAMIC_COPY);
        }
    }
    void GetDataSync() {
        std::vector<PengIntShaderStructs::Vertex> vertices;
        std::vector<PengIntShaderStructs::Triangle> triangles;

        float cx = 0, cy = 0, cz = -5;
        float cw = 1, cx_rot = 0, cy_rot = 0, cz_rot = 0;

        for (int i = 0; i < PengIntShaderStructs::Object::OBJECTS.size(); i++) {
            auto* obj = PengIntShaderStructs::Object::OBJECTS[i];

            auto v_data = obj->GetVertexData(i, vertices.size());
            vertices.insert(vertices.end(), v_data.begin(), v_data.end());

            auto t_data = obj->GetTriangleData(cx, cy, cz, cw, cx_rot, cy_rot, cz_rot);
            triangles.insert(triangles.end(), t_data.begin(), t_data.end());
        }

        SyncGPUData(vertices, triangles);
    }
    void SetDataSync() {
        std::vector<PengIntShaderStructs::Vertex> results(totalVertices);
        rlReadShaderBuffer(vertexSSBO, results.data(), totalVertices * sizeof(PengIntShaderStructs::Vertex), 0);

        for (auto& v : results) {
            if (v.oi < PengIntShaderStructs::Object::OBJECTS.size()) {
                auto* obj = PengIntShaderStructs::Object::OBJECTS[v.oi];
                int baseIdx = v.vi*3;
                if (baseIdx + 2 < obj->Verticies.size()) {
                    obj->Verticies[baseIdx] = v.px;
                    obj->Verticies[baseIdx+1] = v.py;
                    obj->Verticies[baseIdx+2] = v.pz;
                }
            }
        }
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
    void OnEnd() {
        rlUnloadShaderBuffer(vertexSSBO);
        rlUnloadShaderBuffer(triangleSSBO);
        rlUnloadShaderBuffer(zBufferSSBO);
    }
    virtual void OnUpdate_GUI(float dt, float t) { }
    void PreUpdate_UI(float dt, float t) {
        GetDataSync();
        rlEnableShader(RotateProgram);
        int vCountLoc = rlGetLocationUniform(RotateProgram, "count");
        rlSetUniform(vCountLoc, &totalVertices, SHADER_UNIFORM_INT, 1);
        rlBindShaderBuffer(vertexSSBO, 0);
        rlComputeShaderDispatch((totalVertices/256) + 1, 1, 1);
        rlDisableShader();
        rlEnableShader(BufferProgram);
        int tCountLoc = rlGetLocationUniform(BufferProgram, "count");
        rlSetUniform(tCountLoc, &totalTriangles, SHADER_UNIFORM_INT, 1);
        rlBindShaderBuffer(triangleSSBO, 0);
        rlBindShaderBuffer(zBufferSSBO, 1);
        rlBindShaderBuffer(vertexSSBO, 2);
        rlComputeShaderDispatch((totalTriangles/256) + 1, 1, 1);
        rlDisableShader();
        SetDataSync();

        BeginShaderMode(DrawShader);
            rlBindShaderBuffer(zBufferSSBO, 0);
            DrawRectangle(0, 0, WIDTH, HEIGHT, {255, 255, 255, 255});
        EndShaderMode();
        OnUpdate_GUI(dt, t);
    }
};


#endif //PENGINT_GUIL_HPP