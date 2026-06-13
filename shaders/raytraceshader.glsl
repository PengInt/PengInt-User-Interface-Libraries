#version 430

layout(local_size_x = 16, local_size_y = 16) in;

struct Vertex {
    float px, py, pz, _pad1;
    float cx, cy, cz, _pad2;
    float w, x, y, z;
};

struct Triangle {
    int i1, i2, i3, m;
};

struct LightSource {
    float x, y, z, _pad1;
    int r, g, b, a;
};

struct Material {
    float reflectivity, transparency, density, glow;
    int r, g, b, a;
};

layout(location = 0) uniform int triangleCount;
layout(location = 1) uniform int screenWidth;
layout(location = 2) uniform int screenHeight;
layout(location = 3) uniform int lightCount;
layout(location = 4) uniform vec3 camPos;
layout(location = 5) uniform vec4 camRot;
layout(std430, binding = 0) buffer TriangleInput { Triangle data[]; };
layout(std430, binding = 2) buffer RotatedVertexInput { Vertex vData[]; };
layout(std430, binding = 3) buffer ColourBuffer { uint colourValues[]; } outColour;
layout(std430, binding = 4) buffer MaterialInput { Material mdata[]; };
layout(std430, binding = 5) buffer LightSourceInput { LightSource lsdata[]; };

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
    vec3 Uv = Q.xyz/m;
    vec4 Uq = vec4(sin(Q.w/2)*Uv, cos(Q.w/2));
    vec4 Uqc = vec4(-Uq.xyz, Uq.w);

    vec4 N = hamilton(hamilton(Uq, QP), Uqc);
    return N.xyz;
}

struct RayHit {
    int tri_i; float d; vec3 loc;
};

bool Hit(vec3 a, vec3 b, vec3 c, vec3 n, vec3 p) {
    float A = dot(cross(b-a, p-a), n);
    float B = dot(cross(c-b, p-b), n);
    float C = dot(cross(a-c, p-c), n);
    if ((A >= 0 && B >= 0 && C >= 0) || (A <= 0 && B <= 0 && C <= 0)) return true;
    return false;
}

vec3 f(vec3 origin, vec3 direction, float d) {
    return vec3(direction.x*d+origin.x, direction.y*d+origin.y, direction.z*d+origin.z);
}

float LightRaycast(vec3 origin, vec3 direction, LightSource ls, int triSource_i) {
    RayHit closestHit = RayHit(-1, -1, vec3(0, 0, 0));
    for (int i = 0; i < triangleCount; i++) {
        if (i == triSource_i) continue;
        Triangle tri = data[i];
        vec3 a = vec3(vData[tri.i1].px+vData[tri.i1].cx, vData[tri.i1].py+vData[tri.i1].cy, vData[tri.i1].pz+vData[tri.i1].cz);
        vec3 b = vec3(vData[tri.i2].px+vData[tri.i2].cx, vData[tri.i2].py+vData[tri.i2].cy, vData[tri.i2].pz+vData[tri.i2].cz);
        vec3 c = vec3(vData[tri.i3].px+vData[tri.i3].cx, vData[tri.i3].py+vData[tri.i3].cy, vData[tri.i3].pz+vData[tri.i3].cz);
        vec3 normal = normalize(cross(b-a, c-a));
        float A = normal.x;
        float B = normal.y;
        float C = normal.z;
        float D = A*a.x + B*a.y + C*a.z;
        float d = -(A*origin.x + B*origin.y + C*origin.z - D)/(A*direction.x + B*direction.y + C*direction.z);
        vec3 p = f(origin, direction, d);
        if (Hit(a, b, c, normal, p) && (d < closestHit.d || closestHit.tri_i == -1) && d >= 0) {
            closestHit = RayHit(i, d, p);
        }
    }
    if (closestHit.tri_i == -1 || closestHit.d > length(vec3(ls.x, ls.y, ls.z)-origin)) return length(vec3(ls.x, ls.y, ls.z)-origin);
    else return -1;
}

float visibleEffect = 0.025;

struct RaycastResult {
    vec3 colour, light; RayHit hit; vec3 reflection_direction, refraction_direction;
    bool refl, refr;
};

float CorrectHue(float h) {
    h /= 255.0f;
    float a = 2.51; float b = 0.03; float c = 2.43; float d = 0.59; float e = 0.14;
    h = (h*(a*h + b))/(h*(c*h + d) + e);
    return min(1, max(0, h)) * 255;
}

RaycastResult Raycast(vec3 origin, vec3 direction, int skip) {
    RayHit closestHit = RayHit(-1, -1, vec3(0, 0, 0));
    for (int i = 0; i < triangleCount; i++) {
        if (i == skip) continue;
        Triangle tri = data[i];
        vec3 a = vec3(vData[tri.i1].px+vData[tri.i1].cx, vData[tri.i1].py+vData[tri.i1].cy, vData[tri.i1].pz+vData[tri.i1].cz);
        vec3 b = vec3(vData[tri.i2].px+vData[tri.i2].cx, vData[tri.i2].py+vData[tri.i2].cy, vData[tri.i2].pz+vData[tri.i2].cz);
        vec3 c = vec3(vData[tri.i3].px+vData[tri.i3].cx, vData[tri.i3].py+vData[tri.i3].cy, vData[tri.i3].pz+vData[tri.i3].cz);
        vec3 normal = normalize(cross(b-a, c-a));
        float A = normal.x;
        float B = normal.y;
        float C = normal.z;
        float D = A*a.x + B*a.y + C*a.z;
        float d = -(A*origin.x + B*origin.y + C*origin.z - D)/(A*direction.x + B*direction.y + C*direction.z);
        vec3 p = f(origin, direction, d);
        if (Hit(a, b, c, normal, p) && (d < closestHit.d || closestHit.tri_i == -1) && d >= 0) {
            closestHit = RayHit(i, d, p);
        }
    }
    vec3 finalColour = vec3(0, 0, 0);
    bool willRefl = false; bool willRefr = false;
    vec3 refl = vec3(0, 0, 0); vec3 refr = vec3(0, 0, 0);
    if (closestHit.tri_i != -1) {
        Triangle firstHit = data[closestHit.tri_i];
        vec3 a = vec3(vData[firstHit.i1].px+vData[firstHit.i1].cx, vData[firstHit.i1].py+vData[firstHit.i1].cy, vData[firstHit.i1].pz+vData[firstHit.i1].cz);
        vec3 b = vec3(vData[firstHit.i2].px+vData[firstHit.i2].cx, vData[firstHit.i2].py+vData[firstHit.i2].cy, vData[firstHit.i2].pz+vData[firstHit.i2].cz);
        vec3 c = vec3(vData[firstHit.i3].px+vData[firstHit.i3].cx, vData[firstHit.i3].py+vData[firstHit.i3].cy, vData[firstHit.i3].pz+vData[firstHit.i3].cz);
        vec3 normal = normalize(cross(b-a, c-a));
        vec3 colourFromLights = vec3(0, 0, 0);
        for (int i = 0; i < lightCount; i++) {
            vec3 dir = normalize(vec3(lsdata[i].x, lsdata[i].y, lsdata[i].z)-closestHit.loc);
            float lightDist = LightRaycast(closestHit.loc, dir, lsdata[i], closestHit.tri_i);
            if (lightDist != -1) {
                float dotProduct = dot(dir, normal);
                float angleCos = max(dotProduct, -dotProduct);
                colourFromLights += vec3(lsdata[i].r, lsdata[i].g, lsdata[i].b)*lsdata[i].a*angleCos/lightDist;
            }
        }
        Material material = mdata[int(data[closestHit.tri_i].m)];
        finalColour = vec3(material.r, material.g, material.b) * (colourFromLights/255);
        if (material.reflectivity != 0) {
            willRefl = true;
            refl = direction-2*dot(direction, normal)*normal;
        }
        if (material.transparency != 0) {
            willRefr = true;
            refr = direction;
        }
    }
    vec3 lightAddition = vec3(0, 0, 0);
    for (int i = 0; i < lightCount; i++) {
        LightSource l = lsdata[i];
        vec3 dlp = vec3(l.x, l.y, l.z) - origin;
        if (LightRaycast(origin, direction, l, -1) != -1) {
            float ddot = dot(dlp, direction);
            float ffactor = 1;
            if (ddot <= 0) ffactor = exp(ddot*2);
            float aveff = l.a*visibleEffect*ffactor;
            lightAddition += vec3(l.r, l.g, l.b)*aveff/length(dlp-direction*max(0, ddot));
        }
    }
    float highest = max(max(finalColour.r, finalColour.g), finalColour.b);
    if (highest > 255) {
        highest /= 255;
        finalColour.r /= highest;
        finalColour.g /= highest;
        finalColour.b /= highest;
    }
    finalColour = vec3(uint(finalColour.r), uint(finalColour.g), uint(finalColour.b));
    return RaycastResult(finalColour, lightAddition, closestHit, refl, refr, willRefl, willRefr);
}

vec3 RaycastHandler(vec3 origin, vec3 direction) {
    vec3 c = vec3(0, 0, 0);
    Material lastMat = Material(1, 1, 1, 0, 0, 0, 0, 0);
    vec3 lc = vec3(0, 0, 0);
    float currentRefl = 1;
    int skip = -1;
    for (int i = 0; i < 16; i++) {
        RaycastResult RR = Raycast(origin, direction, skip);
        c += RR.colour*currentRefl-lc*currentRefl+RR.light;
        if (RR.refl == false) break;
        lc = RR.colour*currentRefl;
        lastMat = mdata[int(data[RR.hit.tri_i].m)];
        currentRefl *= mdata[int(data[RR.hit.tri_i].m)].reflectivity;
        origin = RR.hit.loc;
        direction = RR.reflection_direction;
        skip = RR.hit.tri_i;
    }
    /*float highest = max(max(c.r, c.g), c.b);
    if (highest > 255) {
        highest /= 255;
        c.r /= highest;
        c.g /= highest;
        c.b /= highest;
    }*/
    /*c.r = min(255, c.r);
    c.g = min(255, c.g);
    c.b = min(255, c.b);*/
    c.r = CorrectHue(c.r);
    c.g = CorrectHue(c.g);
    c.b = CorrectHue(c.b);
    return c;
}

void main() {
    uint idx = gl_GlobalInvocationID.x;
    uint idy = gl_GlobalInvocationID.y;
    uint pixelIdx = idy*screenWidth+idx;
    if (idx >= screenWidth || idy >= screenHeight) return;
    vec4 n_camRot = vec4(camRot.y, camRot.z, camRot.w, -camRot.x);

    Triangle tri = data[0];
    vec3 c = RaycastHandler(camPos, rotate(n_camRot.w, n_camRot.xyz, normalize(vec3(float(idx*2)/screenWidth-1, float(int(idy*2)-int(screenHeight))/screenWidth, 1))));
    uint r = uint(c.x);
    uint g = uint(c.y);
    uint b = uint(c.z);
    uint colour = (uint(255) << 24) | (b << 16) | (g << 8) | r;
    outColour.colourValues[pixelIdx] = colour;
}