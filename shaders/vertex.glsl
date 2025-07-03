
#version 330 core
layout(location = 0) in vec3 inPos;
void main() {
    gl_Position = vec4(
        inPos.x / 450.0 - 1.0,
        inPos.y / 505.0 - 1.0,
        inPos.z,
        1.0
    ); // convierte coordenadas 0–900 y 0–1010 a clip space
}
