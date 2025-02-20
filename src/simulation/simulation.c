#include "simulation.h"
#include "../gl/glh.h"
#include "../gl/shader.h"
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define WORK_GROUP_SIZE 64

void simulation_init(Simulation *simulation, unsigned int seed, uint16_t size, uint8_t groups_count, uint16_t groups_sizes[])
{
  simulation->size = size;
  simulation->last_time = 0.0f;
  // simulation->groups_count = groups_count;
  // simulation->groups_sizes = groups_sizes;
  simulation->particles = calloc(size, sizeof(Particle));

  if (seed == 0)
  {
    seed = time(NULL);
  }
  srand(seed);

  for (uint8_t i = 0; i < groups_count; i++)
  {
    for (uint16_t j = 0; j < groups_sizes[i]; j++)
    {
      simulation->particles[j].type = i;
      simulation->particles[j].position = (GLVec2){(float)rand() / RAND_MAX, (float)rand() / RAND_MAX};
    }
  }

  GLuint vao;
  glCreateVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint particles_buffer;
  glCreateBuffers(1, &particles_buffer);
  glNamedBufferData(particles_buffer, size * sizeof(Particle), simulation->particles, GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particles_buffer);

  Shader compute_shader;
  shader_init(&compute_shader, GL_COMPUTE_SHADER);
  shader_compile(&compute_shader, "./src/simulation/shader/shader.comp");

  ShaderProgram compute_program;
  shader_program_init(&compute_program);
  shader_program_attach(&compute_program, &compute_shader);
  shader_program_link(&compute_program);
  shader_destroy(&compute_shader);
  simulation->compute_program = compute_program;

  Shader vert_shader, frag_shader;
  shader_init(&vert_shader, GL_VERTEX_SHADER);
  shader_compile(&vert_shader, "./src/simulation/shader/shader.vert");
  shader_init(&frag_shader, GL_FRAGMENT_SHADER);
  shader_compile(&frag_shader, "./src/simulation/shader/shader.frag");

  ShaderProgram render_program;
  shader_program_init(&render_program);
  shader_program_attach(&render_program, &vert_shader);
  shader_program_attach(&render_program, &frag_shader);
  shader_program_link(&render_program);
  shader_destroy(&vert_shader);
  shader_destroy(&frag_shader);
  simulation->render_program = render_program;
}

void simulation_update(Simulation *simulation)
{
  float current_time = glfwGetTime();
  float delta_time;
  if (simulation->last_time == 0.0f)
  {
    delta_time = 0.0f;
  }
  else
  {
    delta_time = current_time - simulation->last_time;
  }
  simulation->last_time = current_time;

  shader_program_bind(&simulation->compute_program);
  glUniform1f(glGetUniformLocation(simulation->compute_program.handle, "deltaTime"), delta_time);
  glDispatchCompute(simulation->size / WORK_GROUP_SIZE, 1, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  shader_program_bind(&simulation->render_program);
  glDrawArraysInstanced(GL_POINTS, 0, 1, simulation->size);
}

void simulation_destroy(Simulation *simulation)
{
  free(simulation->particles);
  simulation->size = 0;
}
