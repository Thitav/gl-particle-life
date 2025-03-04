#include <stdio.h>
#include <stdlib.h>

#include "gl/glh.h"
#include "simulation/simulation.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Window resize callback function
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}

int main()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Test", NULL, NULL);
  if (window == NULL)
  {
    fprintf(stderr, "[ERROR] Failed to create GLFW window\n");
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwMakeContextCurrent(window);

  int version = gladLoadGL(glfwGetProcAddress);
  if (version == 0)
  {
    fprintf(stderr, "[ERROR] Failed to initialize OpenGL context\n");
    glfwTerminate();
    return EXIT_FAILURE;
  }

  printf("Loaded OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  Simulation simulation;
  simulation_init(&simulation, 1, 64, 2, (uint16_t[]){32, 32});

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glPointSize(2.0f);

  while (!glfwWindowShouldClose(window))
  {
    glClear(GL_COLOR_BUFFER_BIT);
    simulation_update(&simulation);

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  simulation_destroy(&simulation);
  glfwDestroyWindow(window);
  glfwTerminate();
  return EXIT_SUCCESS;
}
