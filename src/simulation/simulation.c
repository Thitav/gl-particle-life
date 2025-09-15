#include "simulation.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <gl/types.h>
#include <gl/shader.h>
#include "group.h"

#define WORK_GROUP_SIZE 64

int _simulation_shaders_init(SimulationGlData *simulation_gl_data)
{
  GlShader compute_shader;
  if (gl_shader_init(&compute_shader, GL_COMPUTE_SHADER) != 0 ||
      gl_shader_compile(&compute_shader,
                        "E:\\Projetos\\gl-particle-life\\src\\simulation\\shader\\shader.comp") != 0)
  {
    fprintf(stderr, "[ERROR][SIMULATION] Error initializing compute shader\n");
    return -1;
  }

  GlShaderProgram compute_program;
  if (gl_shader_program_init(&compute_program) != 0)
  {
    fprintf(stderr, "[ERROR][SIMULATION] Error initializing compute shader program\n");
    return -1;
  }
  gl_shader_program_attach(&compute_program, &compute_shader);
  if (gl_shader_program_link(&compute_program) != 0)
  {
    fprintf(stderr, "[ERROR][SIMULATION] Error linking compute shader program\n");
    return -1;
  }
  gl_shader_destroy(&compute_shader);

  GlShader vert_shader, frag_shader;
  if (gl_shader_init(&vert_shader, GL_VERTEX_SHADER) != 0 ||
      gl_shader_compile(&vert_shader,
                        "E:\\Projetos\\gl-particle-life\\src\\simulation\\shader\\shader.vert") != 0)
  {
    fprintf(stderr, "[ERROR][SIMULATION] Error initializing vertex shader\n");
    return -1;
  }
  if (gl_shader_init(&frag_shader, GL_FRAGMENT_SHADER) != 0 ||
      gl_shader_compile(&frag_shader,
                        "E:\\Projetos\\gl-particle-life\\src\\simulation\\shader\\shader.frag") != 0)
  {
    fprintf(stderr, "[ERROR][SIMULATION] Error initializing fragment shader\n");
    return -1;
  }

  GlShaderProgram render_program;
  if (gl_shader_program_init(&render_program) != 0)
  {
    fprintf(stderr, "[ERROR][SIMULATION] Error initializing render program\n");
    return -1;
  }

  gl_shader_program_attach(&render_program, &vert_shader);
  gl_shader_program_attach(&render_program, &frag_shader);
  gl_shader_destroy(&vert_shader);
  gl_shader_destroy(&frag_shader);

  if (gl_shader_program_link(&render_program) != 0)
  {
    fprintf(stderr, "[ERROR][SIMULATION] Error linking render program\n");
    return -1;
  }

  simulation_gl_data->compute_program = compute_program;
  simulation_gl_data->render_program = render_program;

  return 0;
}

void _simulation_gl_data_init(SimulationGlData *simulation_gl_data)
{
  _simulation_shaders_init(simulation_gl_data);
  glCreateVertexArrays(1, &simulation_gl_data->vao);
  glCreateBuffers(1, &simulation_gl_data->simulation_ubo);
  glCreateBuffers(1, &simulation_gl_data->particles_ssbo);
}

void simulation_init(Simulation *simulation, SimulationParameters simulation_parameters)
{
  simulation->last_time = 0.0f;

  simulation->gl_data.simulation_data.viscosity = simulation_parameters.viscosity;
  if (simulation_parameters.seed == 0)
  {
    simulation->seed = time(NULL);
  }
  else
  {
    simulation->seed = simulation_parameters.seed;
  }

  _simulation_gl_data_init(&simulation->gl_data);
  _simulation_groups_init(&simulation->groups);
}

SimulationError simulation_add_group(Simulation *simulation, uint32_t group_size)
{
  return _simulation_groups_add_group(&simulation->groups, group_size);
}

SimulationError simulation_add_group_rules(Simulation *simulation, const uint8_t group_idx, const uint16_t rules_count,
                                           const GLS_ParticleGroupRule *rules)
{
  return _simulation_groups_add_rules(&simulation->groups, group_idx, rules_count, rules);
}

SimulationError simulation_start(Simulation *simulation)
{
  if (simulation->groups.group_count == 0)
  {
    return E_SIM_NO_GROUPS;
  }
  if (!_simulation_groups_is_sealed(&simulation->groups))
  {
    return E_SIM_NO_RULES;
  }

  srand(simulation->seed);

  simulation->gl_data.simulation_data.particles_count = simulation->groups.total_size;
  simulation->gl_data.simulation_data.groups_count = simulation->groups.group_count;
  memcpy(simulation->gl_data.simulation_data.groups_rules, simulation->groups.rules,
         simulation->groups.group_count * simulation->groups.group_count * sizeof(GLfloat));

  simulation->gl_data.particles = calloc(simulation->groups.total_size, sizeof(GLS_Particle));
  if (simulation->gl_data.particles == NULL)
  {
    return E_SIM_ALLOC;
  }

  uint32_t counter = 0;
  for (uint8_t i = 0; i < simulation->groups.group_count; i++)
  {
    for (uint32_t j = 0; j < simulation->groups.groups[i].size; j++)
    {
      simulation->gl_data.particles[counter].type = i;
      simulation->gl_data.particles[counter].position = (GLS_Vec2){
        (GLfloat) rand() * 2.0f / RAND_MAX - 1.0f, (GLfloat) rand() * 2.0f / RAND_MAX - 1.0f
      };
      counter++;
    }
  }

  glBindBuffer(GL_UNIFORM_BUFFER, simulation->gl_data.simulation_ubo);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(GLS_SimulationData), &simulation->gl_data.simulation_data, GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glNamedBufferData(simulation->gl_data.particles_ssbo,
                    (GLsizeiptr) (simulation->gl_data.simulation_data.particles_count * sizeof(GLS_Particle)),
                    simulation->gl_data.particles,
                    GL_STATIC_DRAW
  );

  simulation->last_time = glfwGetTime();

  return E_SIM_SUCCESS;
  // memcpy(simulation_data.groups_colors, groups_colors, groups_count * sizeof(GLS_Vec4));

  // simulation_data.groups_rules[8] = 1.0f;
}

void simulation_update(Simulation *simulation)
{
  const float current_time = glfwGetTime();
  const float delta_time = current_time - simulation->last_time;
  simulation->last_time = current_time;

  glBindVertexArray(simulation->gl_data.vao);
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, simulation->gl_data.simulation_ubo);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, simulation->gl_data.particles_ssbo);

  gl_shader_program_bind(&simulation->gl_data.compute_program);
  glUniform1f(glGetUniformLocation(simulation->gl_data.compute_program.handle, "deltaTime"), delta_time);

  glDispatchCompute(simulation->gl_data.simulation_data.particles_count / WORK_GROUP_SIZE, 1, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  gl_shader_program_bind(&simulation->gl_data.render_program);
  glDrawArraysInstanced(GL_POINTS, 0, 1, simulation->gl_data.simulation_data.particles_count);
}

void simulation_destroy(Simulation *simulation)
{
  free(simulation->gl_data.particles);
  // simulation->particles_count = 0;
}
