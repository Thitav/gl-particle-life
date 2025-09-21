#include "group.h"
#include <stdlib.h>
#include <string.h>

#define GROUP_ALLOC_BATCH 8

void simulation_groups_init(SimulationGroups *simulation_groups) {
    simulation_groups->total_size = 0;
    simulation_groups->group_count = 0;
    simulation_groups->groups = NULL;
    simulation_groups->rules = NULL;
    simulation_groups->colors = NULL;
}

void simulation_groups_clear(SimulationGroups *simulation_groups) {
    simulation_groups->total_size = 0;
    simulation_groups->group_count = 0;

    free(simulation_groups->groups);
    free(simulation_groups->rules);
    free(simulation_groups->colors);
    simulation_groups->groups = NULL;
    simulation_groups->rules = NULL;
    simulation_groups->colors = NULL;
}

bool simulation_groups_is_sealed(const SimulationGroups *simulation_groups) {
    return simulation_groups->rules != NULL;
}

Error simulation_groups_add_group(SimulationGroups *simulation_groups, const uint32_t size) {
    ERROR_ASSERT(!simulation_groups_is_sealed(simulation_groups), E_SIM_GROUPS_SEALED);
    ERROR_ASSERT(simulation_groups->group_count < SIMULATION_MAX_GROUPS, E_SIM_GROUPS_MAX_GROUPS);
    ERROR_ASSERT(simulation_groups->total_size + size <= SIMULATION_MAX_PARTICLES,
                 E_SIM_GROUPS_MAX_PARTICLES);

    if (simulation_groups->groups == NULL) {
        simulation_groups->groups = malloc(sizeof(ParticleGroup) * GROUP_ALLOC_BATCH);
        ERROR_ASSERT(simulation_groups->groups != NULL, E_ALLOC);
    } else if (simulation_groups->group_count + 1 % GROUP_ALLOC_BATCH == 0) {
        ParticleGroup *groups_realloc = realloc(
            simulation_groups->groups,
            sizeof(ParticleGroup) * (simulation_groups->group_count / GROUP_ALLOC_BATCH + 1));
        ERROR_ASSERT(groups_realloc != NULL, E_ALLOC);
        simulation_groups->groups = groups_realloc;
    }

    simulation_groups->groups[simulation_groups->group_count] =
        (ParticleGroup){.idx = simulation_groups->group_count, .size = size};
    simulation_groups->group_count++;
    simulation_groups->total_size += size;
    return E_SUCCESS;
}

Error simulation_groups_add_parameters(SimulationGroups *simulation_groups, const uint8_t group_idx,
                                       const GLS_Vec4 group_color, const uint16_t rules_count,
                                       const GLS_ParticleGroupRule *rules) {
    ERROR_ASSERT(rules_count == simulation_groups->group_count, E_SIM_GROUP_RULES_INVALID_COUNT);

    if (simulation_groups->rules == NULL) {
        simulation_groups->rules =
            calloc((size_t)(simulation_groups->group_count * simulation_groups->group_count),
                   sizeof(GLS_ParticleGroupRule));
        ERROR_ASSERT(simulation_groups->rules != NULL, E_ALLOC);
    }
    if (simulation_groups->colors == NULL) {
        simulation_groups->colors = calloc(simulation_groups->group_count, sizeof(GLS_Vec4));
        ERROR_ASSERT(simulation_groups->colors != NULL, E_ALLOC);
    }

    memcpy(&simulation_groups->rules[(size_t)(group_idx * simulation_groups->group_count)], rules,
           simulation_groups->group_count * sizeof(GLS_ParticleGroupRule));
    simulation_groups->colors[group_idx] = group_color;

    return E_SUCCESS;
}
