#version 430

layout(location = 0) uniform int count;

struct Vertex {
    float px, py, pz;
    float cx, cy, cz;
    float w, x, y, z;
    int oi, vi;
    float padding[2];
};

layout(std430, binding=0) buffer VertexBufer { Vertex data[]; };

layout(local_size_x = 256) in;

void main() {
    uint idx = gl_GlobalInvocationID.x;
    if (idx >= count) return;

    // ... math ...

    data[idx].px = rotatedX;
    data[idx].py = rotatedY;
    data[idx].pz = rotatedZ;
}