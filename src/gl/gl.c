#include "gl.h"
#include "../error.h"
#include "GLFW/glfw3.h"
#include "glh.h"

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

Error gl_init(GlState *gl_state, int window_width, int window_height, const char *window_title,
              GLFWerrorfun glfw_error_callback) {
    ERROR_ASSERT(glfwInit() == GLFW_TRUE, E_GL_INIT_GLFW);

    if (glfw_error_callback != NULL) {
        glfwSetErrorCallback(glfw_error_callback);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, SIM_GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, SIM_GL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, SIM_GL_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(window_width, window_height, window_title, NULL, NULL);
    ERROR_ASSERT_DO(window != NULL, E_GL_INIT_WINDOW, glfwTerminate());

    glfwMakeContextCurrent(window);

    int version = gladLoadGL(glfwGetProcAddress);
    ERROR_ASSERT_DO(version != 0, E_GL_INIT_CONTEXT, glfwTerminate());
    ERROR_ASSERT_DO(GLAD_VERSION_MAJOR(version) == SIM_GL_VERSION_MAJOR &&
                        GLAD_VERSION_MINOR(version) == SIM_GL_VERSION_MINOR,
                    E_GL_INIT_VERSION, glfwTerminate());

    glViewport(0, 0, window_width, window_height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    gl_state->window = window;
    return E_SUCCESS;
}

void gl_terminate(GlState *gl_state) {
    glfwDestroyWindow(gl_state->window);
    gl_state->window = NULL;
    glfwTerminate();
}
