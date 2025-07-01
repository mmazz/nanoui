#include "ui.h"
#include "font.h"
#include "camera.h"
#include "platform.h"
#include "renderer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GLFW/glfw3.h>
#define SCR_WIDTH 800
#define SCR_HEIGHT 600

extern GLuint font_text, text_vao, text_vbo;
static char debugTitle[256];
void ortho_matrix(float* out, float left, float right, float bottom, float top) {
    float rl = right - left;
    float tb = top - bottom;

    out[0] = 2.0f / rl;  out[1] = 0.0f;        out[2] = 0.0f; out[3] = 0.0f;
    out[4] = 0.0f;       out[5] = -2.0f / tb;  out[6] = 0.0f; out[7] = 0.0f;
    out[8] = 0.0f;       out[9] = 0.0f;        out[10] = -1;  out[11] = 0.0f;
    out[12] = -(right + left) / rl;
    out[13] = (top + bottom) / tb;
    out[14] = 0.0f;       out[15] = 1.0f;
}
int main(){

    GLFWwindow* window = setup_window(SCR_WIDTH, SCR_HEIGHT, "Simulator");
    if (!window) {
        printf("No windows created\n");
        return -1;
    }
    if (!init_glad()) {
        printf("Error at glad init\n");
        return -1;
    }

    Camera camera = camera_init(cameraPos, cameraUp, YAW, PITCH);
    set_up_callbacks(window, &camera);

    GLuint shaderText = init_shader_program("shaders/textVertex.glsl", "shaders/textFragment.glsl");
    glUseProgram(shaderText);
    glUniform1i(glGetUniformLocation(shaderText, "tex"),   0);
    glUniform4f(glGetUniformLocation(shaderText, "color"), 1,1,1,1);

    // Proyección ortográfica (coordenadas en píxeles)
    float proj[16];
    ortho_matrix(proj, 0.0f, SCR_WIDTH, SCR_HEIGHT, 0.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderText, "proj"),
                       1, GL_FALSE, proj);

    // Ahora inicializamos la fuente
    init_font_texture("assets/font.ttf");
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float deltaTime = 0;
    float lastFrame = 0;
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // Limpiar pantalla
        glClearColor(0.1, 0.1, 0.1, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderText);
        render_text("Test visible", 300, 300, 1.0f, 1.0f);
        processInputMovement(window, deltaTime);
        glfwSetWindowTitle(window, debugTitle);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


