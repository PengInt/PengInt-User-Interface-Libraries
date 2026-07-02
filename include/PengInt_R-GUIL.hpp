#ifndef PENGINT_R_GUIL_HPP
#define PENGINT_R_GUIL_HPP

#include <cstdint>
#include <cstdio>
#include <array>
#include <cmath>
#include <string>
#include <map>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include "PengInt_UIL.hpp"


std::array<float, 3> NormaliseV(const std::array<float, 3>& v) {
    float root = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
    return {v[0]/root, v[1]/root, v[2]/root};
}
std::array<float, 4> NormaliseQ(const std::array<float, 4>& v) {
    float root = sqrt(v[1]*v[1]+v[2]*v[2]+v[3]*v[3]);
    return {v[0], v[1]/root, v[2]/root, v[3]/root};
}

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

std::array<float, 4> hamilton(std::array<float, 4> a, std::array<float, 4> b) {
    std::array<float, 3> cross = VectorCrossProduct({a[1], a[2], a[3]}, {b[1], b[2], b[3]});
    return {
        a[0] * b[0] - VectorDotProduct({a[1], a[2], a[3]}, {b[1], b[2], b[3]}),
        a[0]*b[1]+a[1]*b[0]+cross[0],
        a[0]*b[2]+a[2]*b[0]+cross[1],
        a[0]*b[3]+a[3]*b[0]+cross[2]
    };
}

std::array<float, 3> Rotate(const std::array<float, 3>& v, const std::array<float, 4>& q) {
    if (q[0] == 0) return v;
    std::array<float, 4> qp = {0, v[0], v[1], v[2]};
    std::array<float, 3> uv = NormaliseV({q[1], q[2], q[3]});
    float sinw = sin(q[0]/2);
    std::array<float, 4> uq = {(float) cos(q[0]/2), sinw*uv[0], sinw*uv[1], sinw*uv[2]};
    std::array<float, 4> uqc = {uq[0], -uq[1], -uq[2], -uq[3]};
    std::array<float, 4> n = hamilton(hamilton(uq, qp), uqc);
    return {n[1], n[2], n[3]};
}
std::array<float, 3> RotateAround(const std::array<float, 3>& v, const std::array<float, 3>& c, const std::array<float, 4>& q) {
    std::array<float, 3> r = Rotate({v[0]-c[0], v[1]-c[1], v[2]-c[2]}, q);
    return {r[0]+c[0], r[1]+c[1], r[2]+c[2]};
}

namespace PengIntShaderStructs {
    struct Vertex {
        float px, py, pz, _pad1;
        float cx, cy, cz, _pad2;
        float w, x, y, z;
    };
    struct Triangle_t {
        int i1, i2, i3, m, index;
    };
    struct Triangle {
        int i1, i2, i3, m;
        Triangle(Triangle_t t) { i1 = t.i1; i2 = t.i2; i3 = t.i3; m = t.m; }
    };
    struct BVH_Node_t {
        float count, x0, y0, z0;
        float x1, y1, z1; 
        int index; BVH_Node_t* bvh_ptr; Triangle_t* tri_ptr;
    };
    struct BVH_Node {
        float count, x0, y0, z0;
        float x1, y1, z1, first;
        BVH_Node(BVH_Node_t t) { count = t.count; x0 = t.x0; y0 = t.y0; z0 = t.z0; x1 = t.x1; y1 = t.y1; z1 = t.z1; if (t.bvh_ptr != nullptr) first = t.bvh_ptr->index; else first = -t.tri_ptr->index; }
    };
    struct LightSource {
        float x, y, z, _pad1;
        int r, g, b, a;
    };
    std::vector<LightSource*> LIGHTSOURCES;
    struct Material {
        float reflectivity, transparency, density, glow;
        int r, g, b, a;
    };
    std::vector<Material*> MATERIALS;
    class Object;
    std::map<std::string, std::vector<Object*>> OBJECTS_SORTED;
    std::vector<Object*> OBJECTS;
    std::map<std::string, Object*> NAMED_OBJECTS;
    class Object {
    public:
        float X, Y, Z;
        std::vector<float> Vertices;
        std::vector<int> Triangles;
        std::vector<BVH_Node_t> BVH_Nodes;
        std::array<float, 4> rotation;
        Object(float x, float y, float z, std::vector<float>& v, std::vector<int>& t, std::string name) : X(x), Y(y), Z(z), Vertices(v), Triangles(t) {
            std::string coord = std::to_string((int) floor(x/10)*10) + "," + std::to_string((int) floor(y/10)*10) + "," + std::to_string((int) floor(z/10)*10);
            OBJECTS_SORTED[coord].push_back(this);
            OBJECTS.push_back(this);
            NAMED_OBJECTS.push_back({name, this});
            rotation = {0, 1, 0, 0};
        }
        std::vector<Vertex> GetVertexData() {
            rotation[0] = std::remainder(rotation[0], 2*std::numbers::pi);
            rotation = NormaliseQ(rotation);
            std::vector<Vertex> output;
            for (int i = 0; i < Vertices.size(); i += 3) output.push_back({
                Vertices[i], Vertices[i+1], Vertices[i+2], 0,
                X, Y, Z, 0,
                rotation[0], rotation[1], rotation[2], rotation[3]
            });
            return output;
        }
        std::vector<Triangle_t> GetTriangleData(int offset, int t_offset) {
            std::vector<Triangle_t> output;
            for (int i = 0; i < Triangles.size(); i += 4) output.push_back({
                Triangles[i] + offset, Triangles[i+1] + offset, Triangles[i+2] + offset, Triangles[i+3], i/4 + t_offset
            });
            return output;
        }
        std::vector<BVH_Node_t> GetBVHData(int offset) {
            std::vector<BVH_Node_t> output;
            for (int i = 0; i < BVH_Nodes.size(); i++) {
                BVH_Node_t t = BVH_Nodes[i];
                t.index = offset+i;
                output.push_back(t);
            }
            return output;
        }
    };
    Object* FindObject(std::string name) { if (NAMED_OBJECTS[name]) return NAMED_OBJECTS[name]; return nullptr; }
    void DESTROY_EVERYTHING() {
        for (LightSource* ls : LIGHTSOURCES) delete ls;
        LIGHTSOURCES.clear();
        for (Material* mat : MATERIALS) delete mat;
        MATERIALS.clear();
        for (Object* obj : OBJECTS) delete obj;
        OBJECTS.clear();
        OBJECTS_SORTED.clear();
        NAMED_OBJECTS.clear();
    }
}

std::vector<PengIntShaderStructs::Material> DeRefMat(std::vector<PengIntShaderStructs::Material*> mat_ptr_vec) {
    std::vector<PengIntShaderStructs::Material> flat;
    for (auto ptr : mat_ptr_vec) flat.push_back(*ptr);
    return flat;
}
std::vector<PengIntShaderStructs::LightSource> DeRefLS(std::vector<PengIntShaderStructs::LightSource*> ls_ptr_vec) {
    std::vector<PengIntShaderStructs::LightSource> flat;
    for (auto ptr : ls_ptr_vec) flat.push_back(*ptr);
    return flat;
}

class Renderer : public Window {
protected:
    std::array<float, 3> CameraPosition;
    float CameraPitch, CameraYaw, CameraRoll;
    std::array<float, 4> CameraRotation;
private:
    Shader DrawShader;
    unsigned int RotateProgram;
    unsigned int RaytraceProgram;
    unsigned int vertexSSBO;
    unsigned int triangleSSBO;
    unsigned int cBufferSSBO;
    unsigned int materialSSBO;
    unsigned int lightSourceSSBO;
    int totalVertices = 0;
    int totalTriangles = 0;
    int totalMaterials = 0;
    int totalLightSources = 0;
    void LoadShaders() {
        DrawShader = LoadShader(0, "shaders/drawshader.glsl");

        char* rotCode = LoadFileText("shaders/rotateshader.glsl");
        unsigned int rotShader = rlLoadShader(rotCode, RL_COMPUTE_SHADER);
        RotateProgram = rlLoadShaderProgramCompute(rotShader);
        UnloadFileText(rotCode);

        char* bufCode = LoadFileText("shaders/raytraceshader.glsl");
        unsigned int bufShader = rlLoadShader(bufCode, RL_COMPUTE_SHADER);
        RaytraceProgram = rlLoadShaderProgramCompute(bufShader);
        UnloadFileText(bufCode);

        vertexSSBO = 0;
        triangleSSBO = 0;
        cBufferSSBO = 0;
        materialSSBO = 0;
        lightSourceSSBO = 0;

        CameraPosition = {0, 0, 0};
        CameraPitch = 0;
        CameraYaw = 0;
        CameraRoll = 0;
        CameraRotation = {0, 1, 0, 0};
    }
    void SyncGPUData(const std::vector<PengIntShaderStructs::Vertex>& vertices, const std::vector<PengIntShaderStructs::Triangle>& triangles, const std::vector<PengIntShaderStructs::BVH_Node>& bvh_nodes) {
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

        if (cBufferSSBO == 0) cBufferSSBO = rlLoadShaderBuffer(WIDTH*HEIGHT*sizeof(uint32_t), NULL, RL_DYNAMIC_COPY);

        if (materialSSBO == 0) materialSSBO = rlLoadShaderBuffer(PengIntShaderStructs::MATERIALS.size() * sizeof(PengIntShaderStructs::Material), DeRefMat(PengIntShaderStructs::MATERIALS).data(), RL_DYNAMIC_COPY);
        else if (totalMaterials < PengIntShaderStructs::MATERIALS.size()) {
            rlUnloadShaderBuffer(materialSSBO);
            materialSSBO = rlLoadShaderBuffer(PengIntShaderStructs::MATERIALS.size() * sizeof(PengIntShaderStructs::Material), DeRefMat(PengIntShaderStructs::MATERIALS).data(), RL_DYNAMIC_COPY);
        } else rlUpdateShaderBuffer(materialSSBO, DeRefMat(PengIntShaderStructs::MATERIALS).data(), PengIntShaderStructs::MATERIALS.size() * sizeof(PengIntShaderStructs::Material), 0);
        totalMaterials = PengIntShaderStructs::MATERIALS.size();

        if (lightSourceSSBO == 0) lightSourceSSBO = rlLoadShaderBuffer(PengIntShaderStructs::LIGHTSOURCES.size() * sizeof(PengIntShaderStructs::Material), DeRefLS(PengIntShaderStructs::LIGHTSOURCES).data(), RL_DYNAMIC_COPY);
        else if (totalLightSources < PengIntShaderStructs::LIGHTSOURCES.size()) {
            rlUnloadShaderBuffer(lightSourceSSBO);
            lightSourceSSBO = rlLoadShaderBuffer(PengIntShaderStructs::LIGHTSOURCES.size() * sizeof(PengIntShaderStructs::LightSource), DeRefLS(PengIntShaderStructs::LIGHTSOURCES).data(), RL_DYNAMIC_COPY);
        } else rlUpdateShaderBuffer(lightSourceSSBO, DeRefLS(PengIntShaderStructs::LIGHTSOURCES).data(), PengIntShaderStructs::LIGHTSOURCES.size() * sizeof(PengIntShaderStructs::LightSource), 0);
        totalLightSources = PengIntShaderStructs::LIGHTSOURCES.size();
    }
    void GetDataSync() {
        std::vector<PengIntShaderStructs::Vertex> vertices;
        std::vector<PengIntShaderStructs::Triangle> triangles;
        std::vector<PengIntShaderStructs::Triangle_t> triangles_t;
        std::vector<PengIntShaderStructs::BVH_Node> bvh_nodes;
        std::vector<PengIntShaderStructs::BVH_Node_t> bvh_nodes_t;
        int offset = 0; int offset_t = 0; int offset_bvh = 0;
        for (int i = 0; i < PengIntShaderStructs::OBJECTS.size(); i++) {
            auto* obj = PengIntShaderStructs::OBJECTS[i];
            std::vector<PengIntShaderStructs::Vertex> v_data = obj->GetVertexData();
            vertices.insert(vertices.end(), v_data.begin(), v_data.end());
            std::vector<PengIntShaderStructs::Triangle_t> t_data = obj->GetTriangleData(offset, offset_t);
            triangles_t.insert(triangles_t.end(), t_data.begin(), t_data.end());
            std::vector<PengIntShaderStructs::BVH_Node_t> bvh_data = obj->GetBVHData(offset_bvh);
            bvh_nodes_t.insert(bvh_nodes_t.end(), bvh_data.begin(), bvh_data.end());
            offset += obj->Vertices.size()/3;
            offset_t += obj->Triangles.size()/4;
            offset_bvh += obj->BVH_Nodes.size();
        }
        for (int i = 0; i < triangles_t.size(); i++) triangles.push_back({triangles_t[i]});
        for (int i = 0; i < bvh_nodes_t.size(); i++) bvh_nodes.push_back({bvh_nodes_t[i]});
        SyncGPUData(vertices, triangles, bvh_nodes);
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
    }
    virtual void OnUpdate_GUI(float dt, float t) { }
    void PreUpdate_UI(float dt, float t) {
        ClearBackground({0, 0, 0, 255});
        int sw = WIDTH; int sh = HEIGHT;
        if (IsWindowResized()) {
            sw = GetScreenWidth();
            sh = GetScreenHeight();
            rlUnloadShaderBuffer(cBufferSSBO);
            cBufferSSBO = rlLoadShaderBuffer(sw*sh*sizeof(uint32_t), nullptr, RL_DYNAMIC_COPY);
        }
        GetDataSync();
        rlEnableShader(RotateProgram);
            int vCountLoc = rlGetLocationUniform(RotateProgram, "count");
            rlSetUniform(vCountLoc, &totalVertices, SHADER_UNIFORM_INT, 1);
            rlBindShaderBuffer(vertexSSBO, 0);
            rlComputeShaderDispatch((totalVertices/256) + 1, 1, 1);
        rlDisableShader();
        std::vector<uint32_t> clearC(sw * sh, 0);
        rlUpdateShaderBuffer(cBufferSSBO, clearC.data(), clearC.size() * sizeof(uint32_t), 0);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        rlEnableShader(RaytraceProgram);
            rlSetUniform(rlGetLocationUniform(RaytraceProgram, "triangleCount"), &totalTriangles, SHADER_UNIFORM_INT, 1);
            rlSetUniform(rlGetLocationUniform(RaytraceProgram, "screenWidth"), &sw, SHADER_UNIFORM_INT, 1);
            rlSetUniform(rlGetLocationUniform(RaytraceProgram, "screenHeight"), &sh, SHADER_UNIFORM_INT, 1);
            rlSetUniform(rlGetLocationUniform(RaytraceProgram, "camPos"), &CameraPosition, SHADER_UNIFORM_VEC3, 1);
            CameraRotation = CombineQuaternions({CameraPitch, 1, 0, 0}, {CameraYaw, 0, 1, 0});
            rlSetUniform(rlGetLocationUniform(RaytraceProgram, "camRot"), &CameraRotation, SHADER_UNIFORM_VEC4, 1);
            rlSetUniform(rlGetLocationUniform(RaytraceProgram, "lightCount"), &totalLightSources, SHADER_UNIFORM_INT, 1);
            rlBindShaderBuffer(triangleSSBO, 0);
            rlBindShaderBuffer(vertexSSBO, 2);
            rlBindShaderBuffer(cBufferSSBO, 3);
            rlBindShaderBuffer(materialSSBO, 4);
            rlBindShaderBuffer(lightSourceSSBO, 5);
            rlComputeShaderDispatch((sw+15)/16, (sh+15)/16, 1);
        rlDisableShader();
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
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
PengIntShaderStructs::Material LoadMaterialFromJSON_np(const char* fpath) {
    FILE* fp = fopen(fpath, "rb");
    if (!fp) printf("no file");
    char readbuffer[65536];
    rapidjson::FileReadStream is(fp, readbuffer, sizeof(readbuffer));
    rapidjson::Document doc;
    doc.ParseStream(is);
    fclose(fp);
    if (doc.HasParseError()) printf("parse error");
    assert(doc.IsObject());
    std::vector<float> temp_d;
    if (doc.HasMember("d") && doc["d"].IsArray()) {
        const auto& arr = doc["d"].GetArray();
        temp_d.reserve(arr.Size());
        for (auto& v : arr) {
            if (v.IsFloat()) {
                temp_d.push_back(v.GetFloat());
            }
        }
    }
    return {temp_d[0], temp_d[1], temp_d[2], temp_d[3], (int) temp_d[4], (int) temp_d[5], (int) temp_d[6], (int) temp_d[7]};
}
PengIntShaderStructs::Material* LoadMaterialFromJSON(const char* fpath) {
    PengIntShaderStructs::Material* mat = new PengIntShaderStructs::Material(LoadMaterialFromJSON_np(fpath));
    PengIntShaderStructs::MATERIALS.push_back(mat);
    return mat;
}
PengIntShaderStructs::LightSource LoadLightSourceFromJSON_np(const char* fpath) {
    FILE* fp = fopen(fpath, "rb");
    if (!fp) printf("no file");
    char readbuffer[65536];
    rapidjson::FileReadStream is(fp, readbuffer, sizeof(readbuffer));
    rapidjson::Document doc;
    doc.ParseStream(is);
    fclose(fp);
    if (doc.HasParseError()) printf("parse error");
    assert(doc.IsObject());
    std::vector<float> temp_d;
    if (doc.HasMember("d") && doc["d"].IsArray()) {
        const auto& arr = doc["d"].GetArray();
        temp_d.reserve(arr.Size());
        for (auto& v : arr) {
            if (v.IsFloat()) {
                temp_d.push_back(v.GetFloat());
            }
        }
    }
    return {temp_d[0], temp_d[1], temp_d[2], 0, (int) temp_d[3], (int) temp_d[4], (int) temp_d[5], (int) temp_d[6]};
}
PengIntShaderStructs::LightSource* LoadLightSourceFromJSON(const char* fpath) {
    PengIntShaderStructs::LightSource* ls = new PengIntShaderStructs::LightSource(LoadLightSourceFromJSON_np(fpath));
    PengIntShaderStructs::LIGHTSOURCES.push_back(ls);
    return ls;
}


#endif //PENGINT_R_GUIL_HPP