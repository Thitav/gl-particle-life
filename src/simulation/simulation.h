#ifndef SIMULATION_H
#define SIMULATION_H

#include <gl/glh.h>
#include <gl/shader.h>
#include <stdint.h>

typedef struct
{
  float x;
  float y;
} __declspec(align(8)) GLVec2;

typedef struct
{
  GLuint type;
  GLVec2 position;
  GLVec2 velocity;
} Particle;

typedef struct
{
  uint16_t size;
  // uint8_t groups_count;
  // uint16_t *groups_sizes;
  ShaderProgram compute_program;
  ShaderProgram render_program;
  Particle *particles;
  float last_time;
} Simulation;

void simulation_init(Simulation *simulation, unsigned int seed, uint16_t size, uint8_t groups_count, uint16_t groups_sizes[]);
void simulation_update(Simulation *simulation);
void simulation_destroy(Simulation *simulation);

#endif
