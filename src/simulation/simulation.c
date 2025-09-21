#include "simulation.h"
#include "../utils.h"
#include <stdlib.h>
#include <string.h>

#define WORK_GROUP_SIZE 64

static int simulation_shaders_init(SimulationGlData *simulation_gl_data) {
    GlShader compute_shader;
    if (gl_shader_init(&compute_shader, GL_COMPUTE_SHADER) != 0 ||
        gl_shader_compile(&compute_shader, "/workspace/src/shader/3d/shader.comp") != 0) {
        fprintf(stderr, "[ERROR][SIMULATION] Error initializing compute shader\n");
        return -1;
    }

    GlShaderProgram compute_program;
    if (gl_shader_program_init(&compute_program) != 0) {
        fprintf(stderr, "[ERROR][SIMULATION] Error initializing compute shader program\n");
        return -1;
    }
    gl_shader_program_attach(&compute_program, &compute_shader);
    if (gl_shader_program_link(&compute_program) != 0) {
        fprintf(stderr, "[ERROR][SIMULATION] Error linking compute shader program\n");
        return -1;
    }
    gl_shader_destroy(&compute_shader);

    GlShader vert_shader, frag_shader;
    if (gl_shader_init(&vert_shader, GL_VERTEX_SHADER) != 0 ||
        gl_shader_compile(&vert_shader, "/workspace/src/shader/3d/shader.vert") != 0) {
        fprintf(stderr, "[ERROR][SIMULATION] Error initializing vertex shader\n");
        return -1;
    }
    if (gl_shader_init(&frag_shader, GL_FRAGMENT_SHADER) != 0 ||
        gl_shader_compile(&frag_shader, "/workspace/src/shader/3d/shader.frag") != 0) {
        fprintf(stderr, "[ERROR][SIMULATION] Error initializing fragment shader\n");
        return -1;
    }

    GlShaderProgram render_program;
    if (gl_shader_program_init(&render_program) != 0) {
        fprintf(stderr, "[ERROR][SIMULATION] Error initializing render program\n");
        return -1;
    }

    gl_shader_program_attach(&render_program, &vert_shader);
    gl_shader_program_attach(&render_program, &frag_shader);
    gl_shader_destroy(&vert_shader);
    gl_shader_destroy(&frag_shader);

    if (gl_shader_program_link(&render_program) != 0) {
        fprintf(stderr, "[ERROR][SIMULATION] Error linking render program\n");
        return -1;
    }

    simulation_gl_data->compute_program = compute_program;
    simulation_gl_data->render_program = render_program;

    return 0;
}

static void simulation_gl_data_init(SimulationGlData *simulation_gl_data) {
    simulation_shaders_init(simulation_gl_data);
    glCreateVertexArrays(1, &simulation_gl_data->vao);
    glCreateBuffers(1, &simulation_gl_data->simulation_ubo);
    glCreateBuffers(1, &simulation_gl_data->particles_ssbo);
}

void simulation_init(Simulation *simulation, SimulationParameters parameters) {
    // simulation->last_time = 0.0f;

    // simulation->gl_data.simulation_data.viscosity =
    //     simulation_parameters.viscosity;
    simulation->parameters = parameters;
    srand(parameters.seed);

    simulation_gl_data_init(&simulation->gl_data);
    simulation_groups_init(&simulation->groups);
}

// GLuint index;
// const char *name = "particles[0].type";
// index = glGetProgramResourceIndex(simulation->gl_data.compute_program.handle,
//                                   GL_BUFFER_VARIABLE, name);

// // 2. Query the stride
// GLenum props[] = {GL_ARRAY_STRIDE};
// GLint values[1];
// glGetProgramResourceiv(simulation->gl_data.compute_program.handle, GL_BUFFER_VARIABLE, index, 1,
//                        props, sizeof(values), NULL, values);

// printf("stride = %d\n", values[0]);

Error simulation_add_group(Simulation *simulation, uint32_t group_size) {
    return simulation_groups_add_group(&simulation->groups, group_size);
}

Error simulation_add_group_parameters(Simulation *simulation, const uint8_t group_idx,
                                      const GLS_Vec4 group_color, const uint16_t rules_count,
                                      const GLS_ParticleGroupRule *rules) {
    return simulation_groups_add_parameters(&simulation->groups, group_idx, group_color,
                                            rules_count, rules);
}

Error simulation_add_group_parameters_random(Simulation *simulation, const uint8_t group_idx) {
    GLS_Vec4 group_color =
        (GLS_Vec4){RAND_FLOAT(0.0f, 1.0f), RAND_FLOAT(0.0f, 1.0f), RAND_FLOAT(0.0f, 1.0f), 0.5f};

    GLS_ParticleGroupRule rules[SIMULATION_MAX_GROUPS];
    for (uint8_t i = 0; i < simulation->groups.group_count; i++) {
        float min_radius = RAND_GL_COORD();
        float max_radius = RAND_GL_COORD();
        while (min_radius >= max_radius) {
            max_radius = RAND_GL_COORD();
        }
        rules[i] = (GLS_ParticleGroupRule){.attraction = RAND_FLOAT(-1.0f, 1.0f),
                                           .min_radius = min_radius,
                                           .max_radius = max_radius};
    }

    return simulation_groups_add_parameters(&simulation->groups, group_idx, group_color,
                                            simulation->groups.group_count, rules);
}

Error simulation_start(Simulation *simulation) {
    ERROR_ASSERT(simulation->groups.group_count != 0, E_SIM_NO_GROUPS);
    ERROR_ASSERT(simulation_groups_is_sealed(&simulation->groups), E_SIM_NO_RULES);

    // srand(simulation->parameters.seed);

    if (simulation->parameters.mode_3d) {
        GLS_Particle3d *particles = calloc(simulation->groups.total_size, sizeof(GLS_Particle3d));
        ERROR_ASSERT(particles != NULL, E_ALLOC);

        uint32_t idx = 0;
        for (uint8_t i = 0; i < simulation->groups.group_count; i++) {
            for (uint32_t j = 0; j < simulation->groups.groups[i].size; j++) {
                particles[idx].type = i;
                particles[idx].position =
                    (GLS_Vec3){RAND_GL_COORD(), RAND_GL_COORD(), RAND_FLOAT(1.0f, 2.0f)};
                // printf("%f %f", particles[idx].position.x, particles[idx].position.y);
                idx++;
            }
        }
        glNamedBufferData(simulation->gl_data.particles_ssbo,
                          (GLsizeiptr)(simulation->groups.total_size * sizeof(GLS_Particle3d)),
                          particles, GL_STATIC_DRAW);
        free(particles);
        particles = NULL;
    } else {
        GLS_Particle2d *particles = calloc(simulation->groups.total_size, sizeof(GLS_Particle2d));
        ERROR_ASSERT(particles != NULL, E_ALLOC);

        uint32_t idx = 0;
        for (uint8_t i = 0; i < simulation->groups.group_count; i++) {
            for (uint32_t j = 0; j < simulation->groups.groups[i].size; j++) {
                particles[idx].type = i;
                particles[idx].position = (GLS_Vec2){RAND_GL_COORD(), RAND_GL_COORD()};
                // printf("%f %f", particles[idx].position.x, particles[idx].position.y);
                idx++;
            }
        }
        glNamedBufferData(simulation->gl_data.particles_ssbo,
                          (GLsizeiptr)(simulation->groups.total_size * sizeof(GLS_Particle2d)),
                          particles, GL_STATIC_DRAW);
        free(particles);
        particles = NULL;
    }
    // simulation->gl_data.particles = particles;

    GLS_SimulationData simulation_data = {.groups_count = simulation->groups.group_count,
                                          .particles_count = simulation->groups.total_size,
                                          .viscosity = simulation->parameters.viscosity};

    for (uint8_t i = 0; i < simulation->groups.group_count; i++) {
        memcpy(&simulation_data.groups_rules[(size_t)(i * SIMULATION_MAX_GROUPS)],
               &simulation->groups.rules[(size_t)(i * simulation->groups.group_count)],
               simulation->groups.group_count * sizeof(GLS_ParticleGroupRule));
        simulation_data.groups_colors[i] = simulation->groups.colors[i];
    }

    glBindBuffer(GL_UNIFORM_BUFFER, simulation->gl_data.simulation_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GLS_SimulationData), &simulation_data, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    simulation->gl_data.last_frame_time = glfwGetTime();

    return E_SUCCESS;
}

void simulation_update(Simulation *simulation) {
    float current_time = glfwGetTime();
    const float delta_time = current_time - simulation->gl_data.last_frame_time;
    simulation->gl_data.last_frame_time = current_time;

    glBindVertexArray(simulation->gl_data.vao);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, simulation->gl_data.simulation_ubo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, simulation->gl_data.particles_ssbo);

    gl_shader_program_bind(&simulation->gl_data.compute_program);
    glUniform1f(glGetUniformLocation(simulation->gl_data.compute_program.handle, "deltaTime"),
                delta_time);

    glDispatchCompute(simulation->groups.total_size / WORK_GROUP_SIZE, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    gl_shader_program_bind(&simulation->gl_data.render_program);
    glDrawArraysInstanced(GL_POINTS, 0, 1, (GLsizei)simulation->groups.total_size);
}

void simulation_destroy(Simulation *simulation) {
    // free(simulation->gl_data.particles);
    // simulation->particles_count = 0;
}
