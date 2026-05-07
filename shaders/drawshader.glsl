#version 430

layout(std430, binding = 0) buffer ColourBuffer { uint colourValues[]; };

uniform int screenWidth;

out vec4 finalColour;

void main() {
    int x = int(gl_FragCoord.x);
    int y = int(gl_FragCoord.y);

    uint pixelIdx = uint(y * screenWidth + x);
    uint packed = colourValues[pixelIdx];

    float r = float(packed & 0xFFu) / 255.0;
    float g = float((packed >> 8) & 0xFFu) / 255.0;
    float b = float((packed >> 16) & 0xFFu) / 255.0;

    finalColour = vec4(r, g, b, 1.0);
}