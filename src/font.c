#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "font.h"
#include "glad/glad.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define FIRST_CHAR 32
#define NUM_CHARS 96
#define ATLAS_W 512
#define ATLAS_H 512

GLuint font_text, text_vao, text_vbo;
unsigned char ttf_buffer[1<<20];
unsigned char bitmap[ATLAS_W*ATLAS_H];

stbtt_bakedchar cdata[NUM_CHARS]; // ASCII 32..126

void init_font_texture(const char* fontpath) {
    FILE* f = fopen(fontpath, "rb");
    if (!f) { perror(fontpath); exit(1); }
    fread(ttf_buffer, 1, sizeof(ttf_buffer), f);
    fclose(f);

    stbtt_BakeFontBitmap(ttf_buffer,0,32.0f,
                         bitmap, ATLAS_W, ATLAS_H,
                         FIRST_CHAR, NUM_CHARS, cdata);

    glGenTextures(1, &font_text);
    glBindTexture(GL_TEXTURE_2D, font_text);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,ATLAS_W,ATLAS_H,0,
                 GL_RED,GL_UNSIGNED_BYTE,bitmap);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    // swizzle a RGBA blanco
    int swz[4] = {GL_RED,GL_RED,GL_RED,GL_RED};
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swz);

    // VAO/VBO setup
    glGenVertexArrays(1,&text_vao);
    glGenBuffers(1,&text_vbo);
    glBindVertexArray(text_vao);
    glBindBuffer(GL_ARRAY_BUFFER,text_vbo);
    glEnableVertexAttribArray(0); // position
    glEnableVertexAttribArray(1); // uv
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(2*sizeof(float)));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}


void render_text(const char* text, float x, float y, float sx, float sy) {
    glBindTexture(GL_TEXTURE_2D, font_text);
    glBindVertexArray(text_vao);
    glBindBuffer(GL_ARRAY_BUFFER, text_vbo);

    const char* p = text;
    float xpos = x, ypos = y;
    size_t len = strlen(text);
    float* verts = malloc(len * 6 * 4 * sizeof(float));
    if (!verts) return;
    int quads = 0;

    while (*p) {
        int c = *p++;
        if (c < FIRST_CHAR || c >= FIRST_CHAR+NUM_CHARS) continue;

        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(cdata, ATLAS_W, ATLAS_H,
                           c - FIRST_CHAR, &xpos, &ypos, &q, 1);

        float x0 = q.x0 * sx, y0 = q.y0 * sy;
        float x1 = q.x1 * sx, y1 = q.y1 * sy;
        int i = quads * 24;

        verts[i+0] = x0; verts[i+1] = y0; verts[i+2] = q.s0; verts[i+3] = q.t0;
        verts[i+4] = x1; verts[i+5] = y0; verts[i+6] = q.s1; verts[i+7] = q.t0;
        verts[i+8] = x1; verts[i+9] = y1; verts[i+10]= q.s1; verts[i+11]= q.t1;

        verts[i+12]= x0; verts[i+13]= y0; verts[i+14]= q.s0; verts[i+15]= q.t0;
        verts[i+16]= x1; verts[i+17]= y1; verts[i+18]= q.s1; verts[i+19]= q.t1;
        verts[i+20]= x0; verts[i+21]= y1; verts[i+22]= q.s0; verts[i+23]= q.t1;

        quads++;
    }

    // Subir y dibujar
    glBufferData(GL_ARRAY_BUFFER, quads * 6 * 4 * sizeof(float),
                 verts, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, quads * 6);

    free(verts);
}
