#ifndef SHADER_H
#define SHADER_H

#include <stdio.h>
#include <stdlib.h>

#include "glh.h"

typedef struct
{
  GLuint handle;
} Shader;

typedef struct
{
  GLuint handle;
} ShaderProgram;

int shader_init(Shader *shader, GLenum type);
int shader_compile(Shader *shader, char *path);
void shader_destroy(Shader *shader);

int shader_program_init(ShaderProgram *shader_program);
void shader_program_attach(ShaderProgram *shader_program, Shader *shader);
void shader_program_bind(ShaderProgram *shader_program);
int shader_program_link(ShaderProgram *shader_program);
void shader_program_destroy(ShaderProgram *shader_program);

#endif
