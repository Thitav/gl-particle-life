#ifndef SIMULATION_SIMULATION_H
#define SIMULATION_SIMULATION_H

#include "group.h"
#include <gl/shader.h>
#include <gl/types.h>
#include <stdint.h>

typedef struct {
    GLS_Vec2 position;
    GLS_Vec2 velocity;
    GLuint type;
    GLfloat _pad;
} GLS_Particle;

typedef struct {
    GLS_ParticleGroupRule groups_rules[SIMULATION_MAX_GROUPS * SIMULATION_MAX_GROUPS];
    GLS_Vec4 groups_colors[SIMULATION_MAX_GROUPS];
    GLuint particles_count;
    GLuint groups_count;
    GLfloat viscosity;
} GLS_SimulationData;

typedef struct {
    GlShaderProgram compute_program;
    GlShaderProgram render_program;
    GLuint vao;
    GLuint particles_ssbo;
    GLuint simulation_ubo;
    float last_frame_time;
    // GLS_SimulationData simulation_data;
    // GLS_Particle *particles;
} SimulationGlData;

typedef struct {
    float viscosity;
    unsigned int seed;
} SimulationParameters;

typedef struct {
    SimulationGlData gl_data;
    SimulationGroups groups;
    SimulationParameters parameters;
} Simulation;

void simulation_init(Simulation *simulation, SimulationParameters simulation_parameters);

Error simulation_add_group(Simulation *simulation, uint32_t group_size);

Error simulation_add_group_parameters(Simulation *simulation, const uint8_t group_idx,
                                      const GLS_Vec4 group_color, const uint16_t rules_count,
                                      const GLS_ParticleGroupRule *rules);
Error simulation_add_group_parameters_random(Simulation *simulation, const uint8_t group_idx);

Error simulation_start(Simulation *simulation);

void simulation_update(Simulation *simulation);

void simulation_destroy(Simulation *simulation);

#endif
