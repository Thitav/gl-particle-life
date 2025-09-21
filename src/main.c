#include <stdio.h>
#include <stdlib.h>

#include "gl/gl.h"
#include "simulation/simulation.h"
#include <time.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

int main() {
    GlState gl_state;
    gl_init(&gl_state, SCREEN_WIDTH, SCREEN_HEIGHT, "Simulation", NULL);

    Simulation simulation;
    simulation_init(&simulation,
                    (SimulationParameters){.seed = time(NULL), .viscosity = 1.0f, .mode_3d = true});

    simulation_add_group(&simulation, 1024 * 2);
    simulation_add_group(&simulation, 1024 * 2);
    simulation_add_group(&simulation, 1024 * 2);
    simulation_add_group(&simulation, 1024 * 2);
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
