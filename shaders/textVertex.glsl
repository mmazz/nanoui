#version 330 core
layout(location = 0) in vec3 inPos;    // x,y,z en pixeles
layout(location = 1) in vec2 inUV;

uniform mat4 projection;              // ortho(0, W, 0, H, -1, 1)

out vec2 TexCoords;

void main(){
    // transforma de pixeles a clip space
    gl_Position = projection * vec4(inPos, 1.0);
    TexCoords  = inUV;
  //  TexCoords = vec2(inUV.x, 1.0 - inUV.y);
}
