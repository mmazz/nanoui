#ifndef TEXT_H
#define TEXT_H
#include "glad/glad.h"
#include "stb_truetype.h"

#define ATLAS_W 512
#define ATLAS_H 512
#define FIRST_CHAR 32
#define NUM_CHARS 96

typedef struct {
    unsigned char bitmap[ATLAS_W * ATLAS_H];
    GLuint textureID;
    GLuint shaderID;
    GLuint vao;
    GLuint vbo;
    int baseSizePx;
    stbtt_bakedchar cdata[NUM_CHARS];
} Font;


void font_init(Font *font, const char *fontpath, GLuint shader);

void font_render_text(Font *font, const char *text, float x, float y, float scale);
#endif
