#include "font.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glad/glad.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#define MAX_CHARS 256

void font_init(Font *font, const char *fontpath, GLuint shader) {
    FILE *f = fopen(fontpath, "rb");
    if (!f) { perror(fontpath); exit(1); }
    fread(font->bitmap, 1, sizeof(font->bitmap), f);
    fclose(f);

    unsigned char ttf_buffer[1 << 20];
    FILE *ttf_file = fopen(fontpath, "rb");
    if (!ttf_file) { perror(fontpath); exit(1); }
    fread(ttf_buffer, 1, sizeof(ttf_buffer), ttf_file);
    fclose(ttf_file);
    font->shaderID = shader;
    stbtt_BakeFontBitmap(ttf_buffer, 0, 32.0f,
                         font->bitmap, ATLAS_W, ATLAS_H,
                         FIRST_CHAR, NUM_CHARS, font->cdata);

    glGenTextures(1, &font->textureID);
    glBindTexture(GL_TEXTURE_2D, font->textureID);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ATLAS_W, ATLAS_H, 0,
                 GL_RED, GL_UNSIGNED_BYTE, font->bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    int swz[4] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swz);
    // Setup VAO/VBO
    glGenVertexArrays(1, &font->vao);
    glGenBuffers(1, &font->vbo);
    glBindVertexArray(font->vao);
    glBindBuffer(GL_ARRAY_BUFFER, font->vbo);
    glBufferData(GL_ARRAY_BUFFER, 256 * 6 * 5 * sizeof(float),  // 6 vértices × 5 floats × MAX_CHARS
                 NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0); // position
    glEnableVertexAttribArray(1); // uv
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);         // x,y,z
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // u,v


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void font_render_text(Font *font, const char *text, float x, float y, float z, float sx, float sy) {
    float verts[MAX_CHARS * 6 * 5];
    int quads = 0;
    float xpos = x, ypos = y;

    for (size_t i = 0; text[i] && quads < MAX_CHARS; i++) {
        int c = (unsigned char)text[i];
        if (c < FIRST_CHAR || c >= FIRST_CHAR + NUM_CHARS) continue;

        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(font->cdata, ATLAS_W, ATLAS_H,
                           c - FIRST_CHAR,
                           &xpos, &ypos, &q,
                           1);

        float x0 = q.x0 * sx, y0 = q.y0 * sy;
        float x1 = q.x1 * sx, y1 = q.y1 * sy;
        int idx = quads * 30;

        // Triángulo 1
        verts[idx+0]=x0;     verts[idx+1]=y0;     verts[idx+2]=1;
        verts[idx+3]=q.s0;   verts[idx+4]=q.t0;
        verts[idx+5]=x1;     verts[idx+6]=y0;     verts[idx+7]=1;
        verts[idx+8]=q.s1;   verts[idx+9]=q.t0;
        verts[idx+10]=x1;    verts[idx+11]=y1;    verts[idx+12]=1;
        verts[idx+13]=q.s1;  verts[idx+14]=q.t1;
        // Triángulo 2
        verts[idx+15]=x0;    verts[idx+16]=y0;    verts[idx+17]=1;
        verts[idx+18]=q.s0;  verts[idx+19]=q.t0;
        verts[idx+20]=x1;    verts[idx+21]=y1;    verts[idx+22]=1;
        verts[idx+23]=q.s1;  verts[idx+24]=q.t1;
        verts[idx+25]=x0;    verts[idx+26]=y1;    verts[idx+27]=1;
        verts[idx+28]=q.s0;  verts[idx+29]=q.t1;

        quads++;
    }
    if (!quads) return;

    glBindVertexArray(font->vao);
    glBindBuffer(GL_ARRAY_BUFFER, font->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    quads*6*5*sizeof(float), verts);

    glUseProgram(font->shaderID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font->textureID);
    glDrawArrays(GL_TRIANGLES, 0, quads*6);
    glBindVertexArray(0);
}


