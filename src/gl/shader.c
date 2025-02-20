#include "shader.h"
#include <stdio.h>

char *_shader_read(const char *path)
{
  FILE *fptr = fopen(path, "r");
  if (fptr == NULL)
  {
    fprintf(stderr, "[ERROR][SHADER] Failed to open file %s\n", path);
    return NULL;
  }

  int err = fseek(fptr, 0, SEEK_END);
  size_t fsize = ftell(fptr);
  err = fseek(fptr, 0, SEEK_SET);
  if (err != 0)
  {
    fclose(fptr);
    fprintf(stderr, "[ERROR][SHADER] Failed to read file %s\n", path);
    return NULL;
  }

  char *buff = malloc(fsize + 1);
  size_t read = fread(buff, sizeof(char), fsize, fptr);
  if (read != fsize)
  {
    free(buff);
    fclose(fptr);
    fprintf(stderr, "[ERROR][SHADER] Failed to read file %s\n", path);
    return NULL;
  }
  buff[fsize] = 0;

  fclose(fptr);
  return buff;
}

int _shader_compile(GLuint shader, const GLchar *source)
{
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  GLint success;
  GLchar info_log[512];

  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success == GL_FALSE)
  {
    glGetShaderInfoLog(shader, sizeof(info_log), NULL, info_log);
    fprintf(stderr, "[ERROR][SHADER] Compilation failed\n%s\n", info_log);
    glDeleteShader(shader);
    return 0;
  }

  // free(source);
  return 1;
}

int shader_init(Shader *shader, GLenum type)
{
  shader->handle = glCreateShader(type);

  if (shader->handle == 0)
  {
    fprintf(stderr, "[ERROR][SHADER] Failed to create shader\n");
    return 0;
  }

  return 1;
}

int shader_compile(Shader *shader, char *path)
{
  char *source = _shader_read(path);
  if (source == NULL)
  {
    fprintf(stderr, "[ERROR][SHADER] Failed to load shader\n");
    return 0;
  }

  if (!_shader_compile(shader->handle, source))
  {
    free(source);
    fprintf(stderr, "[ERROR][SHADER] Failed to compile shader\n");
    return 0;
  }

  free(source);
  return 1;
}

void shader_destroy(Shader *shader)
{
  glDeleteShader(shader->handle);
}

int shader_program_init(ShaderProgram *shader_program)
{
  shader_program->handle = glCreateProgram();
  if (shader_program->handle == 0)
  {
    fprintf(stderr, "[ERROR][SHADER] Failed to create shader program\n");
    return 0;
  }
  return 1;
}

void shader_program_attach(ShaderProgram *shader_program, Shader *shader)
{
  glAttachShader(shader_program->handle, shader->handle);
}

void shader_program_bind(ShaderProgram *shader_program)
{
  glUseProgram(shader_program->handle);
}

int shader_program_link(ShaderProgram *shader_program)
{
  glLinkProgram(shader_program->handle);

  int success;
  char info_log[512];

  glGetProgramiv(shader_program->handle, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(shader_program->handle, sizeof(info_log), NULL, info_log);
    fprintf(stderr, "[ERROR][SHADER] Program linking failed\n%s\n", info_log);
    return 0;
  }

  return 0;
}

void shader_program_destroy(ShaderProgram *shader_program)
{
  glDeleteProgram(shader_program->handle);
  shader_program->handle = 0;
}
