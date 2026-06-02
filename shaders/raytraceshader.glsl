#version 430

layout(local_size_x = 16, local_size_y = 16) in;

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

layout(location = 0) uniform int count;
layout(location = 1) uniform int screenWidth;
layout(location = 2) uniform int screenHeight;
layout(std430, binding = 0) buffer TriangleInput { Triangle data[]; };
layout(std430, binding = 1) buffer DepthBuffer { int zValues[]; } outZ;
layout(std430, binding = 2) buffer RotatedVertexInput { Vertex vData[]; };
layout(std430, binding = 3) buffer ColourBuffer { uint colourValues[]; } outColour;

vec4 hamilton(vec4 a, vec4 b) {
    return vec4(
    a.w * b.xyz + b.w * a.xyz + cross(a.xyz, b.xyz),
    a.w * b.w - dot(a.xyz, b.xyz)
    );
}

vec3 rotate(float w, vec3 v, vec3 P) {
    vec4 Q = vec4(v.x, v.y, v.z, w);
    vec4 QP = vec4(P, 0);
    if (Q.w == 0) return P;
    float m = length(Q.xyz);
    vec3 Uv = vec3(Q.x, Q.y, Q.z)/m;
    vec4 Uq = vec4(sin(Q.w/2)*Uv, cos(Q.w/2));
    vec4 Uqc = vec4(-Uq.xyz, Uq.w);

    vec4 N = hamilton(hamilton(Uq, QP), Uqc);
    return N.xyz;
}

void main() {
    uint idx = gl_GlobalInvocationID.x;
    uint idy = gl_GlobalInvocationID.y;
    if (idx*idy >= screenWidth*screenHeight) return;

    // logic to be added here soon :D
}