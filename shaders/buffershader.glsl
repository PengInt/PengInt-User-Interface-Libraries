#version 430

layout(local_size_x = 64, local_size_y = 4, local_size_z = 4) in;

struct Vertex {
    float px, py, pz, _pad1;
    float cx, cy, cz, _pad2;
    float w, x, y, z;
    int oi, vi, _pad3[2];
};

struct Triangle {
    int i1, i2, i3, _pad1;
    float r, g, b, _pad2;
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

float EdgeFunc(vec2 a, vec2 b, vec2 c) {
    return (c.x-a.x)*(b.y-a.y)-(c.y-a.y)*(b.x-a.x);
}

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
    uint idz = gl_GlobalInvocationID.z;
    if (idx >= count) return;

    Triangle tri = data[idx];
    vec3 p1 = vec3(vData[tri.i1].px+vData[tri.i1].cx, vData[tri.i1].py+vData[tri.i1].cy, vData[tri.i1].pz+vData[tri.i1].cz);
    vec3 p2 = vec3(vData[tri.i2].px+vData[tri.i2].cx, vData[tri.i2].py+vData[tri.i2].cy, vData[tri.i2].pz+vData[tri.i2].cz);
    vec3 p3 = vec3(vData[tri.i3].px+vData[tri.i3].cx, vData[tri.i3].py+vData[tri.i3].cy, vData[tri.i3].pz+vData[tri.i3].cz);

    vec3 camera = vec3(tri.cx, tri.cy, tri.cz);

    p1 -= camera;
    p2 -= camera;
    p3 -= camera;

    p1 = rotate(tri.w, vec3(tri.x, tri.y, tri.z), p1);
    p2 = rotate(tri.w, vec3(tri.x, tri.y, tri.z), p2);
    p3 = rotate(tri.w, vec3(tri.x, tri.y, tri.z), p3);

    if (p1.z < 0.01 || p2.z < 0.01 || p3.z < 0.01) return;

    float hw = float(screenWidth)*0.5;
    float hh = float(screenHeight)*0.5;
    float sf = 0;
    if (hw > hh) sf = hh; else sf = hw;
    vec2 s1 = vec2((p1.x/p1.z)*sf+hw, (p1.y/p1.z)*sf+hh);
    vec2 s2 = vec2((p2.x/p2.z)*sf+hw, (p2.y/p2.z)*sf+hh);
    vec2 s3 = vec2((p3.x/p3.z)*sf+hw, (p3.y/p3.z)*sf+hh);

    vec2 topleft = min(s1, min(s2, s3));
    vec2 bottomright = max(s1, max(s2, s3));

    int minX = max(0, int(topleft.x));
    int maxX = min(screenWidth - 1, int(bottomright.x));
    int minY = max(0, int(topleft.y));
    int maxY = min(screenHeight - 1, int(bottomright.y));

    for (int y = minY+int(idz); y <= maxY; y+=4) {
        for (int x = minX+int(idy); x <= maxX; x+=4) {
            vec2 p = vec2(float(x), float(y));

            float w1 = EdgeFunc(s2, s3, p);
            float w2 = EdgeFunc(s3, s1, p);
            float w3 = EdgeFunc(s1, s2, p);

            float totalArea = EdgeFunc(s1, s2, s3);

            float bar1 = w1/totalArea;
            float bar2 = w2/totalArea;
            float bar3 = w3/totalArea;

            float pixelZ = (bar1*p1.z)+(bar2*p2.z)+(bar3*p3.z);
            int z_int = int(pixelZ*1000000);
            if ((w1 <= 0 && w2 <= 0 && w3 <= 0) || (w1 >= 0 && w2 >= 0 && w3 >= 0)) {
                uint pixelIdx = uint(y*screenWidth+x);
                int oz = atomicMin(outZ.zValues[pixelIdx], z_int);
                if (z_int < oz) {
                    uint colour = (uint(tri.b) << 16) | (uint(tri.g) << 8) | uint(tri.r);
                    outColour.colourValues[pixelIdx] = colour;
                }
            }
        }
    }
}