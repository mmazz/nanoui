#include "platform.h"

bool isPause = false;
int gFramebufferWidth = 0;
int gFramebufferHeight = 0;

void update_projection_matrix(int width, int height) {
    gRenderCtx.width = width;
    gRenderCtx.height = height;
    glm_ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f, gRenderCtx.projection);
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
        isPause = !isPause;
    }
    if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    gRenderCtx.width = width;
    gRenderCtx.height = height;
    glm_ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f, gRenderCtx.projection);
    glViewport(0, 0, width, height);
}
bool init_glad() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return false;
    }
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        fprintf(stderr, "OpenGL error after gladLoad: %d\n", err);
    }

    // Configuración global de OpenGL
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Version:  %s\n", glGetString(GL_VERSION));

    return true;
}

GLFWwindow* setup_window(int width, int height, const char* title) {
    if (!glfwInit()) return NULL;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);

    if (!init_glad()) {
        printf("Error at glad init\n");
        return NULL;
    }

    // Usar gRenderCtx en lugar de variables separadas
    glfwGetFramebufferSize(window, &gRenderCtx.width, &gRenderCtx.height);
    glViewport(0, 0, gRenderCtx.width, gRenderCtx.height);
    printf("Framebuffer size: %dx%d\n", gRenderCtx.width, gRenderCtx.height);

    set_up_callbacks(window);
    return window;
}

void set_up_callbacks(GLFWwindow* window) {
    glfwSetKeyCallback(window, key_callback);
}
