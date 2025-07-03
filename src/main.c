#include "platform.h"
#include "renderer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "font.h"
#include <cglm/cglm.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


RenderContext gRenderCtx = {
    .width = 800,
    .height = 600
};
int main(){
    GLFWwindow* window = setup_window(gRenderCtx.width, gRenderCtx.height, "Simulator");
    if (!window) {
        printf("No windows created\n");
        return -1;
    }
    GLuint shaderText = init_shader_program("shaders/textVertex.glsl", "shaders/textFragment.glsl");

    Font font;
    font_init(&font, "assets/font.ttf", shaderText);
    stbi_write_png("font_atlas.png", ATLAS_W, ATLAS_H, 1, font.bitmap, ATLAS_W);

    // Setup inicial de projection (ya tenés el tamaño correcto de setup_window)
    glm_ortho(0.0f, (float)gRenderCtx.width, (float)gRenderCtx.height, 0.0f, -1.0f, 1.0f, gRenderCtx.projection);

    // Configurar shader
    glUseProgram(shaderText);
    GLint locProj = glGetUniformLocation(shaderText, "projection");
    GLint locColor = glGetUniformLocation(shaderText, "textColor");
    GLint locTex = glGetUniformLocation(shaderText, "fontAtlas");

    glUniformMatrix4fv(locProj, 1, GL_FALSE, (const float*)gRenderCtx.projection);
    glUniform3f(locColor, 1.0f, 0.0f, 0.0f);
    glUniform1i(locTex, 0);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2, 0.2, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderText);
        glUniformMatrix4fv(locProj, 1, GL_FALSE, (const float*)gRenderCtx.projection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, font.textureID);

        float x_px = 0.5f * (float)gRenderCtx.width;
        float y_px = 0.1f * (float)gRenderCtx.height;

        float scale = 24.0f / (float)font.baseSizePx;
        font_render_text(&font, "Botón", x_px, y_px, scale);

        scale = 54.0f / (float)font.baseSizePx;
        font_render_text(&font, "Botón BIG", x_px, y_px + 50, scale);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
