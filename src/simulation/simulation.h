#ifndef SIMULATION_SIMULATION_H
#define SIMULATION_SIMULATION_H

#include <gl/types.h>
#include <gl/shader.h>
#include <stdint.h>
#include "group.h"

typedef struct
{
  GLS_Vec2 position;
  GLS_Vec2 velocity;
  GLuint type;
  GLfloat _pad;
} GLS_Particle;

typedef struct
{
  GLfloat groups_rules[64];
  GLS_Vec4 groups_colors[8];
  GLuint particles_count;
  GLuint groups_count;
  GLfloat viscosity;
} GLS_SimulationData;

typedef struct
{
  GlShaderProgram compute_program;
  GlShaderProgram render_program;
  GLuint vao;
  GLuint particles_ssbo;
  GLuint simulation_ubo;
  GLS_SimulationData simulation_data;
  GLS_Particle *particles;
} SimulationGlData;

typedef struct
{
  SimulationGlData gl_data;
  SimulationGroups groups;
  float last_time;
  unsigned int seed;
} Simulation;

void simulation_init(Simulation *simulation, SimulationGroups groups, float viscosity,
                     unsigned int seed);

SimulationError simulation_start(Simulation *simulation);

void simulation_update(Simulation *simulation);

void simulation_destroy(Simulation *simulation);

#endif
