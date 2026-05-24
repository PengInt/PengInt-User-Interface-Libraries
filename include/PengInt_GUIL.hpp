#ifndef PENGINT_GUIL_HPP
#define PENGINT_GUIL_HPP

#include <cstdint>
#include <cstdio>
#include <array>
#include <cmath>
#include <string>
#include <map>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include "PengInt_UIL.hpp"


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

std::array<float, 4> CombineQuaternions(const std::array<float, 4>& q1, const std::array<float, 4>& q2) {
    return toAngleAxis(QuaternionMultiplication(fromAngleAxis(q1), fromAngleAxis(q2)));
}

namespace PengIntShaderStructs {
    struct Vertex {
        float px, py, pz, _pad1;
        float cx, cy, cz, _pad2;
        float w, x, y, z;
        int oi, vi, _pad3[2];
    };
    struct Triangle {
        int i1, i2, i3, glow;
        float r, g, b, a;
        float cx, cy, cz, _pad3;
        float w, x, y, z;
    };
    class Object;
    std::map<std::string, std::vector<Object*>> OBJECTS_SORTED;
    std::vector<Object*> OBJECTS;
    class Object {
    public:
        float X, Y, Z;
        std::vector<float> Vertices;
        std::vector<int> Triangles; // i1, i2, i3, glow, r, g, b, a, m
        std::vector<std::array<float, 4>> PLANNED_ROTATIONS;
        Object(float x, float y, float z, std::vector<float>& v, std::vector<int>& t) : X(x), Y(y), Z(z), Vertices(v), Triangles(t) {
            std::string coord = std::to_string((int) floor(x/10)*10) + "," + std::to_string((int) floor(y/10)*10) + "," + std::to_string((int) floor(z/10)*10);
            OBJECTS_SORTED[coord].push_back(this);
            OBJECTS.push_back(this);
        }
        std::vector<Vertex> GetVertexData(int oi) {
            std::vector<Vertex> output;
            std::array<float, 4> quaternion = {0, 1, 0, 0};
            for (int i = 0; i < PLANNED_ROTATIONS.size(); i++) quaternion = CombineQuaternions(quaternion, PLANNED_ROTATIONS[i]);
            PLANNED_ROTATIONS.clear();
            for (int i = 0; i < Vertices.size(); i += 3) output.push_back({
                Vertices[i], Vertices[i+1], Vertices[i+2], 0,
                X, Y, Z, 0,
                quaternion[0], quaternion[1], quaternion[2], quaternion[3],
                oi, i/3, {0}
            });
            return output;
        }
        std::vector<Triangle> GetTriangleData(int offset, float cx, float cy, float cz, float w, float x, float y, float z) {
            std::vector<Triangle> output;
            for (int i = 0; i < Triangles.size(); i += 9) output.push_back({
                Triangles[i] + offset, Triangles[i+1] + offset, Triangles[i+2] + offset, Triangles[i+3],
                (float) Triangles[i+4], (float) Triangles[i+5], (float) Triangles[i+6], (float) Triangles[i+7],
                cx, cy, cz, 0,
                w, x, y, z
            });
            return output;
        }
    };
}

class Renderer : public Window {
protected:
    std::array<float, 3> CameraPosition;
    float CameraPitch, CameraYaw;
    std::array<float, 4> CameraRotation;
private:
    Shader DrawShader;
    unsigned int RotateProgram;
    unsigned int BufferProgram;
    unsigned int vertexSSBO;
    unsigned int triangleSSBO;
    unsigned int zBufferSSBO;
    unsigned int cBufferSSBO;
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
        cBufferSSBO = 0;

        CameraPosition = {0, 0, 0};
        CameraPitch = 0;
        CameraYaw = 0;
        CameraRotation = {0, 1, 0, 0};
    }
    void SyncGPUData(const std::vector<PengIntShaderStructs::Vertex>& vertices, const std::vector<PengIntShaderStructs::Triangle>& triangles) {
        if (vertexSSBO == 0) vertexSSBO = rlLoadShaderBuffer(vertices.size() * sizeof(PengIntShaderStructs::Vertex), vertices.data(), RL_DYNAMIC_COPY);
        else if (totalVertices < vertices.size()) {
            rlUnloadShaderBuffer(vertexSSBO);
            vertexSSBO = rlLoadShaderBuffer(vertices.size() * sizeof(PengIntShaderStructs::Vertex), vertices.data(), RL_DYNAMIC_COPY);
        } else rlUpdateShaderBuffer(vertexSSBO, vertices.data(), vertices.size() * sizeof(PengIntShaderStructs::Vertex), 0);
        totalVertices = vertices.size();

        if (triangleSSBO == 0) triangleSSBO = rlLoadShaderBuffer(triangles.size() * sizeof(PengIntShaderStructs::Triangle), triangles.data(), RL_DYNAMIC_COPY);
        else if (totalTriangles < triangles.size()) {
            rlUnloadShaderBuffer(triangleSSBO);
            triangleSSBO = rlLoadShaderBuffer(triangles.size() * sizeof(PengIntShaderStructs::Triangle), triangles.data(), RL_DYNAMIC_COPY);
        } else rlUpdateShaderBuffer(triangleSSBO, triangles.data(), triangles.size() * sizeof(PengIntShaderStructs::Triangle), 0);
        totalTriangles = triangles.size();

        if (zBufferSSBO == 0) zBufferSSBO = rlLoadShaderBuffer(WIDTH*HEIGHT*sizeof(int), NULL, RL_DYNAMIC_COPY);
        if (cBufferSSBO == 0) cBufferSSBO = rlLoadShaderBuffer(WIDTH*HEIGHT*sizeof(uint32_t), NULL, RL_DYNAMIC_COPY);
    }
    void GetDataSync() {
        std::vector<PengIntShaderStructs::Vertex> vertices;
        std::vector<PengIntShaderStructs::Triangle> triangles;

        float cx = CameraPosition[0], cy = CameraPosition[1], cz = CameraPosition[2];
        CameraRotation = CombineQuaternions({CameraPitch, 1, 0, 0}, {CameraYaw, 0, 1, 0});
        float cw = CameraRotation[0], cx_rot = CameraRotation[1], cy_rot = CameraRotation[2], cz_rot = CameraRotation[3];

        int offset = 0;
        for (int i = 0; i < PengIntShaderStructs::OBJECTS.size(); i++) {
            auto* obj = PengIntShaderStructs::OBJECTS[i];

            auto v_data = obj->GetVertexData(i);
            vertices.insert(vertices.end(), v_data.begin(), v_data.end());

            auto t_data = obj->GetTriangleData(offset, cx, cy, cz, cw, cx_rot, cy_rot, cz_rot);
            triangles.insert(triangles.end(), t_data.begin(), t_data.end());
            offset += ((int) obj->Vertices.size()/3);
        }

        SyncGPUData(vertices, triangles);
    }
    void SetDataSync() {
        std::vector<PengIntShaderStructs::Vertex> results(totalVertices);
        rlReadShaderBuffer(vertexSSBO, results.data(), totalVertices * sizeof(PengIntShaderStructs::Vertex), 0);

        for (auto& v : results) {
            if (v.oi < PengIntShaderStructs::OBJECTS.size()) {
                auto* obj = PengIntShaderStructs::OBJECTS[v.oi];
                int baseIdx = v.vi*3;
                if (baseIdx + 2 < obj->Vertices.size()) {
                    obj->Vertices[baseIdx] = v.px;
                    obj->Vertices[baseIdx+1] = v.py;
                    obj->Vertices[baseIdx+2] = v.pz;
                }
            }
        }
    }
public:
    Renderer(uint16_t w, uint16_t h) : Window(w, h, "PengInt GUI") {
        LoadShaders();
        CLEAR_BACKHROUND = false;
        CameraPosition = {0, 0, -5};
    }
    Renderer(uint16_t w, uint16_t h, const std::string &title) : Window(w, h, title) {
        LoadShaders();
        CLEAR_BACKHROUND = false;
        CameraPosition = {0, 0, -5};
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
        ClearBackground({0, 0, 0, 255});
        if (IsWindowResized()) {
            rlUnloadShaderBuffer(zBufferSSBO);
            rlUnloadShaderBuffer(cBufferSSBO);
            zBufferSSBO = rlLoadShaderBuffer(WIDTH*HEIGHT*sizeof(int), NULL, RL_DYNAMIC_COPY);
            cBufferSSBO = rlLoadShaderBuffer(WIDTH*HEIGHT*sizeof(uint32_t), NULL, RL_DYNAMIC_COPY);
        }
        int sw = WIDTH; int sh = HEIGHT;
        GetDataSync();
        rlEnableShader(RotateProgram);
            int vCountLoc = rlGetLocationUniform(RotateProgram, "count");
            rlSetUniform(vCountLoc, &totalVertices, SHADER_UNIFORM_INT, 1);
            rlBindShaderBuffer(vertexSSBO, 0);
            rlComputeShaderDispatch((totalVertices/256) + 1, 1, 1);
        rlDisableShader();
        std::vector<int> clearZ(sw * sh, 2147483647);
        rlUpdateShaderBuffer(zBufferSSBO, clearZ.data(), clearZ.size() * sizeof(int), 0);
        std::vector<uint32_t> clearC(sw * sh, 0);
        rlUpdateShaderBuffer(cBufferSSBO, clearC.data(), clearC.size() * sizeof(uint32_t), 0);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        rlEnableShader(BufferProgram);
            rlSetUniform(rlGetLocationUniform(BufferProgram, "count"), &totalTriangles, SHADER_UNIFORM_INT, 1);
            rlSetUniform(rlGetLocationUniform(BufferProgram, "screenWidth"), &sw, SHADER_UNIFORM_INT, 1);
            rlSetUniform(rlGetLocationUniform(BufferProgram, "screenHeight"), &sh, SHADER_UNIFORM_INT, 1);
            rlBindShaderBuffer(triangleSSBO, 0);
            rlBindShaderBuffer(zBufferSSBO, 1);
            rlBindShaderBuffer(vertexSSBO, 2);
            rlBindShaderBuffer(cBufferSSBO, 3);
            rlComputeShaderDispatch((totalTriangles/64) + 1, 4, 4);
        rlDisableShader();
        SetDataSync();

        BeginShaderMode(DrawShader);
            int screenWidthLoc = rlGetLocationUniform(DrawShader.id, "screenWidth");
            SetShaderValue(DrawShader, screenWidthLoc, &sw, SHADER_UNIFORM_INT);
            rlBindShaderBuffer(cBufferSSBO, 0);
            DrawRectangle(0, 0, sw, sh, WHITE);
        EndShaderMode();
        OnUpdate_GUI(dt, t);
    }
};

PengIntShaderStructs::Object* LoadObjectFromJSON(const char* fpath) {
    FILE* fp = fopen(fpath, "rb");
    if (!fp) printf("no file");
    char readbuffer[65536];
    rapidjson::FileReadStream is(fp, readbuffer, sizeof(readbuffer));
    rapidjson::Document doc;
    doc.ParseStream(is);
    fclose(fp);
    if (doc.HasParseError()) printf("parse error");
    assert(doc.IsObject());
    std::vector<float> temp_v;
    if (doc.HasMember("v") && doc["v"].IsArray()) {
        const auto& arr = doc["v"].GetArray();
        temp_v.reserve(arr.Size());
        for (auto& v : arr) {
            if (v.IsFloat()) {
                temp_v.push_back(v.GetFloat());
            }
        }
    }
    std::vector<int> temp_t;
    if (doc.HasMember("t") && doc["t"].IsArray()) {
        const auto& arr = doc["t"].GetArray();
        temp_t.reserve(arr.Size());
        for (auto& t : arr) {
            if (t.IsInt()) {
                temp_t.push_back(t.GetInt());
            }
        }
    }
    return new PengIntShaderStructs::Object(0, 0, 0, temp_v, temp_t);
}


#endif //PENGINT_GUIL_HPP