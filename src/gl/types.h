#ifndef GL_TYPES_H
#define GL_TYPES_H

#include "glh.h"

typedef struct {
    GLfloat x;
    GLfloat y;
} GLS_Vec2;

typedef struct {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    // GLfloat _pad;
} GLS_Vec3;

typedef struct {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat w;
} GLS_Vec4;

#endif
