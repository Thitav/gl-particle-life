#ifndef SIMULATION_ERROR_H
#define SIMULATION_ERROR_H

#define ERROR_ASSERT(condition, error)                                                             \
    if (!(condition)) {                                                                            \
        return error;                                                                              \
    }

#define ERROR_ASSERT_DO(condition, error, action)                                                  \
    if (!(condition)) {                                                                            \
        action;                                                                                    \
        return error;                                                                              \
    }

typedef enum {
    E_SUCCESS,
    E_ALLOC,
    E_GL_INIT_GLFW,
    E_GL_INIT_WINDOW,
    E_GL_INIT_CONTEXT,
    E_GL_INIT_VERSION,
    E_SIM_NO_GROUPS,
    E_SIM_NO_RULES,
    E_SIM_GROUPS_SEALED,
    E_SIM_GROUPS_MAX_GROUPS,
    E_SIM_GROUPS_MAX_PARTICLES,
    E_SIM_GROUP_RULES_INVALID_COUNT,
} Error;

#endif
