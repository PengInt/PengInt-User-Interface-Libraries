#version 430

layout(location = 0) uniform int count;

struct Vertex {
    float px, py, pz;
    float cx, cy, cz;
    float w, x, y, z;
    int oi, vi;
    float padding[2];
};

vec4 hamilton(vec4 a, vec4 b) {
    return vec4(
        a.w * b.xyz + b.w * a.xyz + cross(a.xyz, b.xyz),
        a.w * b.w - dot(a.xyz, b.xyz)
    );
}

layout(std430, binding=0) buffer VertexBufer { Vertex data[]; };

layout(local_size_x = 256) in;

void main() {
    uint idx = gl_GlobalInvocationID.x;
    if (idx >= count) return;

    Vertex dataidx = data[idx];
    vec3 P = vec3(dataidx.px, dataidx.py, dataidx.pz);
    vec3 R = vec3(dataidx.cx, dataidx.cy, dataidx.cz);
    vec4 Q = vec4(dataidx.x, dataidx.y, dataidx.z, dataidx.w);

    vec4 QD = vec4(P-R, 0);

    float m = length(Q.xyz);
    vec3 Uv = vec3(Q.x, Q.y, Q.z)/m;
    vec4 Uq = vec4(sin(Q.w/2)*Uv, cos(Q.w/2));
    vec4 Uqc = vec4(-Uq.xyz, Uq.w);

    vec4 N = hamilton(hamilton(Uq, QD), Uqc) + vec4(R, 0);

    data[idx].px = N.x;
    data[idx].py = N.y;
    data[idx].pz = N.z;
}