#version 430

layout(local_size_x = 256) in;

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

layout(std430, binding=0) buffer TriangleInput { Triangle data[]; };
layout(std430, binding=1) buffer ZBufferOutput { float zData[]; };
layout(std430, binding=2) buffer RotatedVertexInput { Vertex vData[]; };

void main() {
    uint idx = gl_GlobalInvocationID.x;
    // ...
}