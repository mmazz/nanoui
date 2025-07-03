#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D fontAtlas;
uniform vec3 textColor;

void main() {
    float alpha = texture(fontAtlas, TexCoord).a;  // Usamos canal rojo como alpha
    FragColor = vec4(textColor, alpha);             // Color configurable con alpha de textura
}
