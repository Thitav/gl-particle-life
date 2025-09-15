#ifndef SIMULATION_GROUP_H
#define SIMULATION_GROUP_H

#include <stdint.h>
#include "error.h"

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
  float *rules;
} SimulationGroups;

void simulation_groups_init(SimulationGroups *simulation_groups);

void simulation_groups_clear(SimulationGroups *simulation_groups);

SimulationError simulation_groups_add_group(SimulationGroups *simulation_groups, uint32_t group_size);

SimulationError simulation_groups_add_rules(SimulationGroups *simulation_groups, uint8_t group_idx,
                                            uint8_t rules_count,
                                            const float *rules);

ParticleGroup *simulation_groups_get(const SimulationGroups *simulation_groups, uint8_t group_idx);

#endif
