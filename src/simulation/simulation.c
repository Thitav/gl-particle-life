#include "simulation.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../gl/glh.h"
#include "../gl/shader.h"

#define WORK_GROUP_SIZE 64

void simulation_init(Simulation *simulation, unsigned int seed, uint16_t particles_count, uint8_t groups_count,
                     uint16_t groups_sizes[], float groups_rules[], GLSL_Vec4 groups_colors[])
{
  simulation->particles_count = particles_count;
  simulation->last_time = 0.0f;
  // simulation->groups_count = groups_count;
  // simulation->groups_sizes = groups_sizes;
  simulation->particles = calloc(particles_count, sizeof(GLSL_Particle));

  if (seed == 0)
  {
    seed = time(NULL);
  }
  srand(seed);

  uint16_t counter = 0;
  for (uint8_t i = 0; i < groups_count; i++)
  {
    for (uint16_t j = 0; j < groups_sizes[i]; j++)
    {
      simulation->particles[counter].type = i;
      simulation->particles[counter].position = (GLSL_Vec2){
        (float) rand() * 2.0f / RAND_MAX - 1.0f, (float) rand() * 2.0f / RAND_MAX - 1.0f
      };
      // simulation->particles[counter].position = (GLSL_Vec2){0.2f, 0.2f};
      counter++;
    }
  }

  GLSL_SimulationData simulation_data = {.particles_count = particles_count, .groups_count = groups_count};
  for (uint16_t i = 0; i < groups_count * groups_count; i += 4)
  {
    simulation_data.groups_rules[i] = (GLSL_Vec4){
      groups_rules[i], groups_rules[i + 1], groups_rules[i + 2], groups_rules[i + 3]
    };
  }
  memcpy(simulation_data.groups_colors, groups_colors, groups_count * sizeof(GLSL_Vec4));

  // simulation_data.groups_rules[8] = 1.0f;

  GLuint vao;
  glCreateVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint simulation_ubo;
  glCreateBuffers(1, &simulation_ubo);
  glBindBuffer(GL_UNIFORM_BUFFER, simulation_ubo);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(GLSL_SimulationData), &simulation_data, GL_STATIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, simulation_ubo);

  GLuint particles_ssbo;
  glCreateBuffers(1, &particles_ssbo);
  glNamedBufferData(particles_ssbo, (GLsizeiptr) (particles_count * sizeof(GLSL_Particle)), simulation->particles,
                    GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particles_ssbo);

  Shader compute_shader;
  shader_init(&compute_shader, GL_COMPUTE_SHADER);
  shader_compile(&compute_shader,
                 "C:\\Users\\thitav\\Documents\\gl-particle-life\\src\\simulation\\shader\\shader.comp");

  ShaderProgram compute_program;
  shader_program_init(&compute_program);
  shader_program_attach(&compute_program, &compute_shader);
  shader_program_link(&compute_program);
  shader_destroy(&compute_shader);
  simulation->compute_program = compute_program;

  Shader vert_shader, frag_shader;
  shader_init(&vert_shader, GL_VERTEX_SHADER);
  shader_compile(&vert_shader, "C:\\Users\\thitav\\Documents\\gl-particle-life\\src\\simulation\\shader\\shader.vert");
  shader_init(&frag_shader, GL_FRAGMENT_SHADER);
  shader_compile(&frag_shader, "C:\\Users\\thitav\\Documents\\gl-particle-life\\src\\simulation\\shader\\shader.frag");

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
  glDispatchCompute(simulation->particles_count / WORK_GROUP_SIZE, 1, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  shader_program_bind(&simulation->render_program);
  glDrawArraysInstanced(GL_POINTS, 0, 1, simulation->particles_count);
}

void simulation_destroy(Simulation *simulation)
{
  free(simulation->particles);
  simulation->particles_count = 0;
}
