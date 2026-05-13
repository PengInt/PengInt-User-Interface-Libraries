#version 430

layout(std430, binding = 0) buffer ColourBuffer { uint colourValues[]; };

uniform int screenWidth;

out vec4 finalColour;

void main() {
    int x = int(gl_FragCoord.x);
    int y = int(gl_FragCoord.y);

    uint pixelIdx = uint(y * 800 + x);
    uint packedVal = colourValues[pixelIdx];

    float r = float(packedVal & 0xFFu) / 255.0;
    float g = float((packedVal >> 8) & 0xFFu) / 255.0;
    float b = float((packedVal >> 16) & 0xFFu) / 255.0;

    finalColour = vec4(r, g, b, 1.0);
}