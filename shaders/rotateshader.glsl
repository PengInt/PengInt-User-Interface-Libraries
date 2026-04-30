#version 430

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
    // ...
}