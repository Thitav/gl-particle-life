#include "group.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "error.h"

#define GROUP_ALLOC_BATCH 8

void _simulation_groups_init(SimulationGroups *simulation_groups)
{
  simulation_groups->total_size = 0;
  simulation_groups->group_count = 0;
  simulation_groups->groups = NULL;
  simulation_groups->rules = NULL;
}

void _simulation_groups_clear(SimulationGroups *simulation_groups)
{
  free(simulation_groups->groups);
  free(simulation_groups->rules);
  simulation_groups->groups = NULL;
  simulation_groups->rules = NULL;

  simulation_groups->total_size = 0;
  simulation_groups->group_count = 0;
}

ParticleGroup *_simulation_groups_get(const SimulationGroups *simulation_groups, const uint8_t group_idx)
{
  if (group_idx > simulation_groups->group_count)
  {
    return NULL;
  }
  return &simulation_groups->groups[group_idx];
}

bool _simulation_groups_is_sealed(const SimulationGroups *simulation_groups)
{
  return simulation_groups->rules != NULL;
}

SimulationError _simulation_groups_add_group(SimulationGroups *simulation_groups, const uint32_t group_size)
{
  if (_simulation_groups_is_sealed(simulation_groups))
  {
    return E_SIM_GROUPS_SEALED;
  }
  if (simulation_groups->group_count >= SIMULATION_MAX_GROUPS)
  {
    return E_SIM_GROUPS_MAX_GROUPS;
  }
  if (simulation_groups->total_size + group_size > SIMULATION_MAX_PARTICLES)
  {
    return E_SIM_GROUPS_MAX_PARTICLES;
  }

  if (simulation_groups->groups == NULL)
  {
    simulation_groups->groups = malloc(sizeof(ParticleGroup) * GROUP_ALLOC_BATCH);
    if (simulation_groups->groups == NULL)
    {
      return E_SIM_ALLOC;
    }
  }
  else if (simulation_groups->group_count + 1 % GROUP_ALLOC_BATCH == 0)
  {
    ParticleGroup *groups_realloc = realloc(simulation_groups->groups,
                                            sizeof(ParticleGroup) * (
                                              simulation_groups->group_count / GROUP_ALLOC_BATCH + 1));
    if (groups_realloc == NULL)
    {
      return E_SIM_ALLOC;
    }
    simulation_groups->groups = groups_realloc;
  }

  simulation_groups->groups[simulation_groups->group_count] = (ParticleGroup){
    .idx = simulation_groups->group_count,
    .size = group_size
  };
  simulation_groups->group_count++;
  simulation_groups->total_size += group_size;
  return E_SIM_SUCCESS;
}

SimulationError _simulation_groups_add_rules(SimulationGroups *simulation_groups, const uint8_t group_idx,
                                             const uint16_t rules_count,
                                             const GLS_ParticleGroupRule *rules)
{
  if (rules_count != simulation_groups->group_count)
  {
    return E_SIM_GROUP_RULES_INVALID_COUNT;
  }

  if (simulation_groups->rules == NULL)
  {
    simulation_groups->rules = calloc(simulation_groups->group_count * simulation_groups->group_count,
                                      sizeof(GLS_ParticleGroupRule));
    if (simulation_groups->rules == NULL)
    {
      return E_SIM_ALLOC;
    }
  }

  memcpy(&simulation_groups->rules[group_idx * simulation_groups->group_count], rules,
         simulation_groups->group_count * sizeof(GLS_ParticleGroupRule));
  return E_SIM_SUCCESS;
}
