#include "ui.h"
#include "camera.h"
#include "platform.h"
#include "renderer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "font.h"
#include <GLFW/glfw3.h>
#define SCR_WIDTH 800
#define SCR_HEIGHT 600
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
extern GLuint font_text, text_vao, text_vbo;
extern GLuint bitmap;
static char debugTitle[256];

const char* vsrc = "#version 330 core\n"
                   "layout(location = 0) in vec2 aPos;\n"
                   "void main() {\n"
                   "  gl_Position = vec4(aPos, 0.0, 1.0);\n"
                   "}\n";

const char* fsrc = "#version 330 core\n"
                   "out vec4 FragColor;\n"
                   "void main() {\n"
                   "  FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
                   "}\n";


GLuint create_shader_from_source(const char* vsrc, const char* fsrc) {
    GLuint v = glCreateShader(GL_VERTEX_SHADER);
    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(v, 1, &vsrc, NULL);
    glShaderSource(f, 1, &fsrc, NULL);
    glCompileShader(v);
    glCompileShader(f);

    GLint success;
    glGetShaderiv(v, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(v, 512, NULL, log);
        fprintf(stderr, "Vertex shader error:\n%s\n", log);
    }

    glGetShaderiv(f, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(f, 512, NULL, log);
        fprintf(stderr, "Fragment shader error:\n%s\n", log);
    }

    GLuint prog = glCreateProgram();
    glAttachShader(prog, v);
    glAttachShader(prog, f);
    glLinkProgram(prog);
    glDeleteShader(v);
    glDeleteShader(f);

    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(prog, 512, NULL, log);
        fprintf(stderr, "Link error:\n%s\n", log);
    }

    return prog;
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
const GLubyte* version = glGetString(GL_VERSION);
if (version)
    printf("OpenGL version: %s\n", version);
else
    fprintf(stderr, "ERROR: glGetString(GL_VERSION) returned NULL\n");
    Camera camera = camera_init(cameraPos, cameraUp, YAW, PITCH);
    set_up_callbacks(window, &camera);

 //   GLuint shaderText = init_shader_program("shaders/textVertex.glsl", "shaders/textFragment.glsl");
    GLuint shaderText = create_shader_from_source(vsrc, fsrc);
    glUseProgram(shaderText);
    if (shaderText == 0) {
    fprintf(stderr, "Error: Failed to initialize text shader\n");
    return -1;
}
    // Ahora inicializamos la fuente
    init_font_texture("assets/font.ttf");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    stbi_write_png("font_atlas.png", 512, 512, 1, &bitmap, 512);

glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

float triangle[] = {
    -0.5f, -0.5f,
     0.5f, -0.5f,
     0.0f,  0.5f,
};

GLuint vao, vbo;
glGenVertexArrays(1, &vao);
glGenBuffers(1, &vbo);
glBindVertexArray(vao);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

    float deltaTime = 0;
    float lastFrame = 0;
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // Limpiar pantalla
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClearColor(0.1, 0.1, 0.1, 1.0);
glClear(GL_COLOR_BUFFER_BIT);

glUseProgram(shaderText);
glBindVertexArray(vao);
glDrawArrays(GL_TRIANGLES, 0, 3);
        render_text("Test visible", 300, 300, 1.0f, 1.0f);
        processInputMovement(window, deltaTime);
        glfwSetWindowTitle(window, debugTitle);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


