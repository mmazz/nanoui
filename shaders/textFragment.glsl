#version 330 core
in vec2 fragUV;
out vec4 fragColor;

uniform sampler2D tex;

void main() {
    fragColor = vec4(fragUV, 0.0, 1.0);  // usa UV como color
}
