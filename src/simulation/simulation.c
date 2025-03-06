#include "simulation.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../gl/glh.h"
#include "../gl/shader.h"

#define WORK_GROUP_SIZE 64

int simulation_shaders_init(SimulationShaders *simulation_shaders)
{
  Shader compute_shader;
  if (shader_init(&compute_shader, GL_COMPUTE_SHADER) != 0 ||
      shader_compile(&compute_shader,
                     "C:\\Users\\thitav\\Documents\\gl-particle-life\\src\\simulation\\shader\\shader.comp") != 0)
  {
    fprintf(stderr, "[ERROR][SIMULATION] Error initializing compute shader\n");
    return -1;
  }

  ShaderProgram compute_program;
  if (shader_program_init(&compute_program) != 0)
  {
    fprintf(stderr, "[ERROR][SIMULATION] Error initializing compute shader program\n");
    return -1;
  }
  shader_program_attach(&compute_program, &compute_shader);
  if (shader_program_link(&compute_program) != 0)
  {
    fprintf(stderr, "[ERROR][SIMULATION] Error linking compute shader program\n");
    return -1;
  }
  shader_destroy(&compute_shader);

  Shader vert_shader, frag_shader;
  if (shader_init(&vert_shader, GL_VERTEX_SHADER) != 0 ||
      shader_compile(&vert_shader,
                     "C:\\Users\\thitav\\Documents\\gl-particle-life\\src\\simulation\\shader\\shader.vert") != 0)
  {
    fprintf(stderr, "[ERROR][SIMULATION] Error initializing vertex shader\n");
    return -1;
  }
  if (shader_init(&frag_shader, GL_FRAGMENT_SHADER) != 0 ||
      shader_compile(&frag_shader,
                     "C:\\Users\\thitav\\Documents\\gl-particle-life\\src\\simulation\\shader\\shader.frag") != 0)
  {
    fprintf(stderr, "[ERROR][SIMULATION] Error initializing fragment shader\n");
    return -1;
  }

  ShaderProgram render_program;
  if (shader_program_init(&render_program) != 0)
  {
    fprintf(stderr, "[ERROR][SIMULATION] Error initializing render program\n");
    return -1;
  }

  shader_program_attach(&render_program, &vert_shader);
  shader_program_attach(&render_program, &frag_shader);
  shader_destroy(&vert_shader);
  shader_destroy(&frag_shader);

  if (shader_program_link(&render_program) != 0)
  {
    fprintf(stderr, "[ERROR][SIMULATION] Error linking render program\n");
    return -1;
  }

  simulation_shaders->compute_program = compute_program;
  simulation_shaders->render_program = render_program;

  return 0;
}

void simulation_init(Simulation *simulation, SimulationShaders simulation_shaders, unsigned int seed,
                     uint16_t particles_count, uint8_t groups_count,
                     uint16_t groups_sizes[], float groups_rules[], GLS_Vec4 groups_colors[])
{
  simulation->particles_count = particles_count;
  simulation->last_time = 0.0f;
  // simulation->groups_count = groups_count;
  // simulation->groups_sizes = groups_sizes;
  simulation->particles = calloc(particles_count, sizeof(GLS_Particle));
  simulation->shaders = simulation_shaders;

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
      simulation->particles[counter].position = (GLS_Vec2){
        (float) rand() * 2.0f / RAND_MAX - 1.0f, (float) rand() * 2.0f / RAND_MAX - 1.0f
      };
      // simulation->particles[counter].position = (GLS_Vec2){0.2f, 0.2f};
      counter++;
    }
  }

  GLS_SimulationData simulation_data = {.particles_count = particles_count, .groups_count = groups_count};
  memcpy(simulation_data.groups_rules, groups_rules, groups_count * groups_count * sizeof(GLfloat));
  memcpy(simulation_data.groups_colors, groups_colors, groups_count * sizeof(GLS_Vec4));

  // simulation_data.groups_rules[8] = 1.0f;

  GLuint vao;
  glCreateVertexArrays(1, &vao);
  simulation->vao = vao;

  GLuint simulation_ubo;
  glCreateBuffers(1, &simulation_ubo);
  glBindBuffer(GL_UNIFORM_BUFFER, simulation_ubo);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(GLS_SimulationData), &simulation_data, GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  simulation->simulation_ubo = simulation_ubo;

  GLuint particles_ssbo;
  glCreateBuffers(1, &particles_ssbo);
  glNamedBufferData(particles_ssbo, (GLsizeiptr) (particles_count * sizeof(GLS_Particle)), simulation->particles,
                    GL_STATIC_DRAW);
  simulation->particles_ssbo = particles_ssbo;
}

void simulation_update(Simulation *simulation)
{
  const float current_time = glfwGetTime();
  if (simulation->last_time == 0.0f)
  {
    simulation->last_time = current_time;
    return;
  }

  const float delta_time = current_time - simulation->last_time;
  simulation->last_time = current_time;

  glBindVertexArray(simulation->vao);
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, simulation->simulation_ubo);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, simulation->particles_ssbo);

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
