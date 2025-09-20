#ifndef SIMULATION_GROUP_H
#define SIMULATION_GROUP_H

#include "error.h"
#include <gl/types.h>
#include <stdbool.h>
#include <stdint.h>

#define SIMULATION_MAX_GROUPS 32
#define SIMULATION_MAX_PARTICLES UINT32_MAX

typedef struct {
    GLfloat attraction;
    GLfloat min_radius;
    GLfloat max_radius;
    GLfloat _pad;
} GLS_ParticleGroupRule;

typedef struct {
    uint8_t idx;
    uint32_t size;
} ParticleGroup;

typedef struct {
    uint32_t total_size;
    uint8_t group_count;
    ParticleGroup *groups;
    GLS_ParticleGroupRule *rules;
    GLS_Vec4 *colors;
} SimulationGroups;

void simulation_groups_init(SimulationGroups *simulation_groups);

void simulation_groups_clear(SimulationGroups *simulation_groups);

bool simulation_groups_is_sealed(const SimulationGroups *simulation_groups);

Error simulation_groups_add_group(SimulationGroups *simulation_groups, uint32_t group_size);

Error simulation_groups_add_parameters(SimulationGroups *simulation_groups, const uint8_t group_idx,
                                       const GLS_Vec4 group_color, const uint16_t rules_count,
                                       const GLS_ParticleGroupRule *rules);

#endif
