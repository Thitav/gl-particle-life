#include <stdio.h>
#include <stdlib.h>

#include "gl/glh.h"
#include "simulation/simulation.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

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

  SimulationGroups groups;
  simulation_groups_init(&groups);
  simulation_groups_add_group(&groups, 500);
  simulation_groups_add_group(&groups, 500);
  simulation_groups_add_group(&groups, 500);
  simulation_groups_add_group(&groups, 500);
  simulation_groups_add_rules(&groups, 0, 4, (float[]){0.9f, -0.8f, 0.3f, 0.06f});
  simulation_groups_add_rules(&groups, 1, 4, (float[]){-0.4f, 0.5f, 0.2f, 0.6f});
  simulation_groups_add_rules(&groups, 2, 4, (float[]){-0.8f, 0.2f, -0.02f, -0.7f});
  simulation_groups_add_rules(&groups, 3, 4, (float[]){0.5f, 0.9f, -0.3f, 0.4f});

  Simulation simulation;
  simulation_init(&simulation, groups, 0.5f, 42);
  simulation_start(&simulation);

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
