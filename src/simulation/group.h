#ifndef SIMULATION_GROUP_H
#define SIMULATION_GROUP_H

#include <stdint.h>
#include "error.h"
#include <gl/types.h>
#include <stdbool.h>

#define SIMULATION_MAX_GROUPS 32
#define SIMULATION_MAX_PARTICLES UINT32_MAX

typedef struct
{
  GLfloat attraction;
  GLfloat min_radius;
  GLfloat max_radius;
  GLfloat _pad;
} GLS_ParticleGroupRule;

typedef struct
{
  uint8_t idx;
  uint32_t size;
} ParticleGroup;

typedef struct
{
  uint32_t total_size;
  uint8_t group_count;
  ParticleGroup *groups;
  GLS_ParticleGroupRule *rules;
} SimulationGroups;

void _simulation_groups_init(SimulationGroups *simulation_groups);

void _simulation_groups_clear(SimulationGroups *simulation_groups);

bool _simulation_groups_is_sealed(const SimulationGroups *simulation_groups);

ParticleGroup *_simulation_groups_get(const SimulationGroups *simulation_groups, uint8_t group_idx);

SimulationError _simulation_groups_add_group(SimulationGroups *simulation_groups, uint32_t group_size);

SimulationError _simulation_groups_add_rules(SimulationGroups *simulation_groups, uint8_t group_idx,
                                             uint16_t rules_count,
                                             const GLS_ParticleGroupRule *rules);


#endif
