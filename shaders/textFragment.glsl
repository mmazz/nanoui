#version 330 core

in vec2 uv;
out vec4 frag_color;

uniform sampler2D tex;
uniform vec4 color;

void main() {
    float alpha = texture(tex, uv).r;
    frag_color = vec4(color.rgb, color.a * alpha);
}
