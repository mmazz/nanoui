#version 330 core
layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inUV;
out vec2 fragUV;

uniform mat4 proj;

void main() {
    fragUV = inUV;
    gl_Position = proj * vec4(inPos, 0.0, 1.0);
}
