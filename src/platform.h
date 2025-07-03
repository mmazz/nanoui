#ifndef PLATFORM_H
#define PLATFORM_H
#include "glad/glad.h"
#include <GLFW/glfw3.h>
//#include "camera.h"
#include <stdio.h>
#include <stdbool.h>

extern bool isPause;
void set_up_callbacks(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
bool init_glad();
GLFWwindow* setup_window(int width, int height, const char* title);

#endif
