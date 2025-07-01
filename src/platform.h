
#ifndef PLATFORM_H
#define PLATFORM_H
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "camera.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


void processInputMovement(GLFWwindow* window, float deltaTime);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


void framebuffer_size_callback(GLFWwindow *window, int width, int height);
bool init_glad();

void set_up_callbacks(GLFWwindow* window, Camera* camera);
GLFWwindow* setup_window(int width, int height, const char* title);

#endif
