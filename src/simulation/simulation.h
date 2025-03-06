#ifndef SIMULATION_H
#define SIMULATION_H

#include <gl/glh.h>
#include <gl/shader.h>
#include <stdint.h>

typedef struct
{
  GLfloat x;
  GLfloat y;
} GLS_Vec2;

typedef struct
{
  GLfloat x;
  GLfloat y;
  GLfloat z;
  GLfloat w;
} GLS_Vec4;

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
} GLS_SimulationData;

typedef struct
{
  ShaderProgram compute_program;
  ShaderProgram render_program;
} SimulationShaders;

typedef struct
{
  SimulationShaders shaders;
  uint16_t particles_count;
  // uint8_t groups_count;
  // uint16_t *groups_sizes;
  ShaderProgram compute_program;
  ShaderProgram render_program;
  GLuint vao;
  GLuint particles_ssbo;
  GLuint simulation_ubo;
  GLS_Particle *particles;
  float last_time;
} Simulation;

void simulation_init(Simulation *simulation, unsigned int seed, uint16_t particles_count, uint8_t groups_count,
                     uint16_t groups_sizes[], float groups_rules[], GLS_Vec4 groups_colors[]);

void simulation_update(Simulation *simulation);

void simulation_destroy(Simulation *simulation);

#endif
