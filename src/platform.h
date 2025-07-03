#ifndef PLATFORM_H
#define PLATFORM_H
#include "glad/glad.h"
#include <GLFW/glfw3.h>
//#include "camera.h"
#include <stdio.h>
#include <stdbool.h>
#include <cglm/cglm.h>

extern bool isPause;
extern int gFramebufferWidth, gFramebufferHeight;

typedef struct {
    int width, height;
    mat4 projection;
    // otros datos globales del render
} RenderContext;

extern RenderContext gRenderCtx;

void update_projection_matrix(int width, int height);
void set_up_callbacks(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
bool init_glad();
GLFWwindow* setup_window(int width, int height, const char* title);

#endif
