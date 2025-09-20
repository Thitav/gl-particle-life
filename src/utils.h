#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

#define RAND_FLOAT(min, max) (((float)rand() / (float)RAND_MAX) * ((max) - (min)) + (min))
#define RAND_GL_COORD() RAND_FLOAT(-1.0f, 1.0f)

#endif
