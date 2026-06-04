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

struct RayHit {
    int tri_i; float d; vec3 loc;
};

bool Hit(vec3 a, vec3 b, vec3 c, vec3 n, vec3 p) {
    if (dot(cross(b-a, p-a), n) >= 0 && dot(cross(c-b, p-b), n) >= 0 && dot(cross(a-c, p-c), n) >= 0) return true;
    return false;
}

vec3 Raycast(vec3 origin, vec3 direction) {
    RayHit closestHit = RayHit(-1, -1, vec3(0, 0, 0));
    for (int i = 0; i < count; i++) {
        Triangle tri = data[i];
        vec3 a = vec3(vData[tri.i1].px+vData[tri.i1].cx, vData[tri.i1].py+vData[tri.i1].cy, vData[tri.i1].pz+vData[tri.i1].cz);
        vec3 b = vec3(vData[tri.i2].px+vData[tri.i2].cx, vData[tri.i2].py+vData[tri.i2].cy, vData[tri.i2].pz+vData[tri.i2].cz);
        vec3 c = vec3(vData[tri.i3].px+vData[tri.i3].cx, vData[tri.i3].py+vData[tri.i3].cy, vData[tri.i3].pz+vData[tri.i3].cz);
        vec3 normal = normalize(cross(b-a, c-a));
        float A = normal.x;
        float B = normal.y;
        float C = normal.z;
        float D = -A*a.x - B*a.y - C*a.z;
        float d = (A*origin.x + B*origin.y + C*origin.z - D)/(A*direction.x + B*direction.y + C*direction.z);
        if (Hit(a, b, c, normal, vec3(direction.x*d-origin.x, direction.y*d-origin.y, direction.z*d-origin.z))) {
            return vec3(255, 255, 255);
        }
    }
    return vec3(0, 0, 0);
}

void main() {
    uint idx = gl_GlobalInvocationID.x;
    uint idy = gl_GlobalInvocationID.y;
    uint pixelIdx = idy*screenWidth+idx;
    if (idx*idy >= screenWidth*screenHeight) return;

    Triangle tri = data[0];
    vec3 c = Raycast(vec3(tri.cx, tri.cy, tri.cz), rotate(tri.w, vec3(tri.x, tri.y, tri.z), normalize(vec3(float(idx*2)/screenWidth-1, float(int(idy*2)-int(screenHeight))/screenWidth, 1))));
    uint r = uint(c.x);
    uint g = uint(c.y);
    uint b = uint(c.z);
    uint colour = (uint(255) << 24) | (b << 16) | (g << 8) | r;
    outColour.colourValues[pixelIdx] = colour;
}