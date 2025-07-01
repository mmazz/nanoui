#version 330 core

layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec2 in_uv;

out vec2 uv;

uniform mat4 proj;

void main() {
    gl_Position = proj * vec4(in_pos, 0.0, 1.0);
    uv = in_uv;
}

