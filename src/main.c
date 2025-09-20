#include <stdio.h>
#include <stdlib.h>

#include "gl/gl.h"
#include "gl/glh.h"
#include "gl/types.h"
#include "simulation.h"
#include <time.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

// Window resize callback function
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    GlState gl_state;
    gl_init(&gl_state, SCREEN_WIDTH, SCREEN_HEIGHT, "Simulation", NULL);

    Simulation simulation;
    simulation_init(&simulation, (SimulationParameters){.seed = time(NULL), .viscosity = 0.5f});

    simulation_add_group(&simulation, 256 * 2);
    simulation_add_group(&simulation, 256 * 2);
    simulation_add_group(&simulation, 256 * 2);
    simulation_add_group(&simulation, 256 * 2);
    // simulation_add_group(&simulation, 500);
    // simulation_add_group(&simulation, 500);
    // simulation_add_group(&simulation, 500);
    // simulation_add_group_parameters(&simulation, 0, (GLS_Vec4){1.0f, 0.0f, 0.0f, 1.0f}, 1,
    //                                 &(GLS_ParticleGroupRule){
    //                                     0.2f,
    //                                     0.0f,
    //                                     1.0f,
    //                                 });
    simulation_add_group_parameters_random(&simulation, 0);
    simulation_add_group_parameters_random(&simulation, 1);
    simulation_add_group_parameters_random(&simulation, 2);
    simulation_add_group_parameters_random(&simulation, 3);
    // simulation_add_group_parameters_random(&simulation, 2);
    // simulation_add_group_parameters_random(&simulation, 3);

    simulation_start(&simulation);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);
    // glPointSize(2.0f);

    while (!glfwWindowShouldClose(gl_state.window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        simulation_update(&simulation);

        glfwPollEvents();
        glfwSwapBuffers(gl_state.window);
    }

    simulation_destroy(&simulation);
    gl_terminate(&gl_state);
    return EXIT_SUCCESS;
}
