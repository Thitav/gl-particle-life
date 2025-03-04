#ifndef SIMULATION_H
#define SIMULATION_H

#include <gl/glh.h>
#include <gl/shader.h>
#include <stdint.h>

typedef struct
{
  GLfloat x;
  GLfloat y;
} GLSL_Vec2;

typedef struct
{
  GLfloat x;
  GLfloat y;
  GLfloat z;
  GLfloat w;
} GLSL_Vec4;

typedef struct
{
  GLSL_Vec2 position;
  GLSL_Vec2 velocity;
  GLuint type;
  GLfloat _pad;
} GLSL_Particle;

typedef struct
{
  GLuint particles_count;
  GLuint groups_count;
  GLfloat _pad[2];
  GLSL_Vec4 groups_colors[8];
  GLSL_Vec4 groups_rules[16];
} GLSL_SimulationData;

typedef struct
{
  uint16_t particles_count;
  // uint8_t groups_count;
  // uint16_t *groups_sizes;
  ShaderProgram compute_program;
  ShaderProgram render_program;
  GLSL_Particle *particles;
  float last_time;
} Simulation;

void simulation_init(Simulation *simulation, unsigned int seed, uint16_t particles_count, uint8_t groups_count,
                     uint16_t groups_sizes[], float groups_rules[], GLSL_Vec4 groups_colors[]);

void simulation_update(Simulation *simulation);

void simulation_destroy(Simulation *simulation);

#endif
