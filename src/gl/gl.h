#ifndef SIM_GL_H
#define SIM_GL_H

#include "../error.h"
#include "glh.h"

#define SIM_GL_VERSION_MAJOR 4
#define SIM_GL_VERSION_MINOR 5
#define SIM_GL_PROFILE GLFW_OPENGL_CORE_PROFILE

typedef struct {
    GLFWwindow *window;
} GlState;

Error gl_init(GlState *gl_state, int window_width, int window_height, const char *window_title,
              GLFWerrorfun glfw_error_callback);

void gl_terminate(GlState *gl_state);

#endif
