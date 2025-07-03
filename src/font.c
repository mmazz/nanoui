#include "font.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glad/glad.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#define MAX_CHARS 256
#define CHAR_SIZE 32.0f

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
    font->baseSizePx = CHAR_SIZE;
    stbtt_BakeFontBitmap(ttf_buffer, 0, CHAR_SIZE,
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
    glBufferData(GL_ARRAY_BUFFER, 256 * 6 * 4 * sizeof(float),  // 6 vértices × 5 floats × MAX_CHARS
                 NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); // position: x, y
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); // texcoord: u, v
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// Estoy usando la proyecccion ortonormal que me manda el (0,0) hacia arriba a la izquierda
// Con eso puedo poner
void font_render_text(Font *font, const char *text, float x, float y, float scale) {
    float verts[MAX_CHARS * 6 * 4];
    int quads = 0;
    float xpos = 0, ypos = 0; // Posición relativa desde 0,0

    for (size_t i = 0; text[i] && quads < MAX_CHARS; i++) {
        int c = (unsigned char)text[i];
        if (c < FIRST_CHAR || c >= FIRST_CHAR + NUM_CHARS) continue;

        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(font->cdata, ATLAS_W, ATLAS_H, c - FIRST_CHAR, &xpos, &ypos, &q, 1);

        // AQUÍ ESTÁ EL FIX: Aplicar escala Y offset
        float x0 = (q.x0 * scale) + x;
        float y0 = (q.y0 * scale) + y;
        float x1 = (q.x1 * scale) + x;
        float y1 = (q.y1 * scale) + y;

        int idx = quads * 6 * 4;

        // Triángulo 1
        verts[idx+ 0] = x0; verts[idx+ 1] = y0; verts[idx+ 2] = q.s0; verts[idx+ 3] = q.t0;
        verts[idx+ 4] = x1; verts[idx+ 5] = y0; verts[idx+ 6] = q.s1; verts[idx+ 7] = q.t0;
        verts[idx+ 8] = x1; verts[idx+ 9] = y1; verts[idx+10] = q.s1; verts[idx+11] = q.t1;

        // Triángulo 2
        verts[idx+12] = x0; verts[idx+13] = y0; verts[idx+14] = q.s0; verts[idx+15] = q.t0;
        verts[idx+16] = x1; verts[idx+17] = y1; verts[idx+18] = q.s1; verts[idx+19] = q.t1;
        verts[idx+20] = x0; verts[idx+21] = y1; verts[idx+22] = q.s0; verts[idx+23] = q.t1;

        quads++;
    }

    if (!quads) return;

    glBindVertexArray(font->vao);
    glBindBuffer(GL_ARRAY_BUFFER, font->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, quads * 6 * 4 * sizeof(float), verts);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font->textureID);
    glDrawArrays(GL_TRIANGLES, 0, quads * 6);
    glBindVertexArray(0);
}

