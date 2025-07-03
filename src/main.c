#include "platform.h"
#include "renderer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "font.h"
#include <cglm/cglm.h>
#include <GLFW/glfw3.h>
#define SCR_WIDTH 800
#define SCR_HEIGHT 600
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


static char debugTitle[256];
GLuint testVAO, testVBO;
void initTestQuad() {
    // un quad de 100×100 píxeles en la pantalla, en (50,50)

float testVerts[] = {
    50.0f,  50.0f, 0.0f, 0.0f, 1.0f,
   150.0f,  50.0f, 0.0f, 1.0f, 1.0f,
   150.0f, 150.0f, 0.0f, 1.0f, 0.0f,

    50.0f,  50.0f, 0.0f, 0.0f, 1.0f,
   150.0f, 150.0f, 0.0f, 1.0f, 0.0f,
    50.0f, 150.0f, 0.0f, 0.0f, 0.0f
};
    glGenVertexArrays(1, &testVAO);
    glGenBuffers(1, &testVBO);
    glBindVertexArray(testVAO);
    glBindBuffer(GL_ARRAY_BUFFER, testVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(testVerts), testVerts, GL_STATIC_DRAW);

    // igual layout que tu texto
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));

    glBindVertexArray(0);
}

int main(){
    GLFWwindow* window = setup_window(SCR_WIDTH, SCR_HEIGHT, "Simulator");
    if (!window) {
        printf("No windows created\n");
        return -1;
    }

    GLint w, h;
    glfwGetFramebufferSize(window, &w, &h);
    printf("Viewport: %d × %d\n", w, h);
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    printf("GL Viewport actual: %d %d %d %d\n", viewport[0], viewport[1], viewport[2], viewport[3]);

    const GLubyte* version = glGetString(GL_VERSION);
    if (version)
        printf("OpenGL version: %s\n", version);
    else
        fprintf(stderr, "ERROR: glGetString(GL_VERSION) returned NULL\n");

    set_up_callbacks(window);

    GLuint shaderText = init_shader_program("shaders/textVertex.glsl", "shaders/textFragment.glsl");
    GLuint shader = init_shader_program("shaders/vertex.glsl", "shaders/fragment.glsl");
    initTestQuad();
    glUseProgram(shaderText);
    if (shaderText == 0) {
        fprintf(stderr, "Error: Failed to initialize text shader\n");
        return -1;
    }
    Font font;
    font_init(&font, "assets/font.ttf", shaderText);
    stbi_write_png("font_atlas.png", ATLAS_W, ATLAS_H, 1, font.bitmap, ATLAS_W);
    glViewport(0, 0,SCR_WIDTH, SCR_HEIGHT );
    mat4 projection;
    glm_ortho(0.0f, SCR_WIDTH, 0.0f, SCR_HEIGHT, -1.0f, 1.0f, projection);

    glUseProgram(shaderText);
    GLint locProj = glGetUniformLocation(shaderText, "projection");
    glUniformMatrix4fv(locProj, 1, GL_FALSE, (const float*)projection);
    // Configurar el color del texto (ej: rojo)
    GLint locColor = glGetUniformLocation(shaderText, "textColor");
    glUniform3f(locColor, 1.0f, 0.0f, 0.0f); // rojo
    GLint locTex = glGetUniformLocation(shaderText, "fontAtlas");
    glUniform1i(locTex, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font.textureID);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2, 0.2, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // <- 🔴 importante: setear de nuevo los uniforms
        glUniform3f(locColor, 1.0f, 0.0f, 0.0f);
        glUniform1i(locTex, 0);


        glUseProgram(shader);
        glBindVertexArray(testVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, font.textureID);  // o cualquier textura blanca 1×1
        glUseProgram(shaderText);
        font_render_text(&font, "Test visible, tratando de entender por que verga pasa", 300, 300, 0.5f, 1.0f, 1.0f);
        glUseProgram(shaderText);

        glfwSetWindowTitle(window, debugTitle);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


