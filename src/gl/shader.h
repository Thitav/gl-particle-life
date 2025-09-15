#ifndef GL_SHADER_H
#define GL_SHADER_H

#include <stdio.h>
#include <stdlib.h>

#include "glh.h"

typedef struct
{
  GLuint handle;
} GlShader;

typedef struct
{
  GLuint handle;
} GlShaderProgram;

int gl_shader_init(GlShader *shader, GLenum type);

int gl_shader_compile(const GlShader *shader, const char *path);

void gl_shader_destroy(const GlShader *shader);

int gl_shader_program_init(GlShaderProgram *shader_program);

void gl_shader_program_attach(const GlShaderProgram *shader_program, const GlShader *shader);

void gl_shader_program_bind(const GlShaderProgram *shader_program);

int gl_shader_program_link(const GlShaderProgram *shader_program);

void gl_shader_program_destroy(GlShaderProgram *shader_program);

#endif
