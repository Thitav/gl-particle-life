#include "shader.h"
#include "glh.h"
#include <stdio.h>

int gl_shader_init(GlShader *shader, GLenum type) {
    shader->handle = glCreateShader(type);

    if (shader->handle == 0) {
        fprintf(stderr, "[ERROR][SHADER] Failed to create shader\n");
        return -1;
    }

    return 0;
}

int gl_shader_compile(const GlShader *shader, const char *path) {
    FILE *fptr = fopen(path, "rb");
    if (fptr == NULL) {
        fprintf(stderr, "[ERROR][SHADER] Failed to open file %s\n", path);
        return -1;
    }

    if (fseek(fptr, 0, SEEK_END) != 0) {
        fclose(fptr);
        fprintf(stderr, "[ERROR][SHADER] Failed to seek to end of file %s\n", path);
        return -1;
    }
    const size_t file_size = ftell(fptr);
    if (fseek(fptr, 0, SEEK_SET) != 0) {
        fclose(fptr);
        fprintf(stderr, "[ERROR][SHADER] Failed to seek to begin of file %s\n", path);
        return -1;
    }

    char *source = malloc(file_size + 1);
    if (source == NULL) {
        fclose(fptr);
        fprintf(stderr, "[ERROR][SHADER] Failed to allocate memory reading file %s\n", path);
        return -1;
    }

    const size_t read = fread(source, sizeof(char), file_size, fptr);
    if (read != file_size) {
        free(source);
        fclose(fptr);
        fprintf(stderr, "[ERROR][SHADER] Failed to read file %s\n", path);
        return -1;
    }

    source[file_size] = 0;
    fclose(fptr);

    glShaderSource(shader->handle, 1, (const GLchar *const *)&source, NULL);
    glCompileShader(shader->handle);
    free(source);

    GLint success;
    glGetShaderiv(shader->handle, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLchar info_log[512];
        glGetShaderInfoLog(shader->handle, sizeof(info_log), NULL, info_log);
        fprintf(stderr, "[ERROR][SHADER] Compilation failed %s\n%s\n", path, info_log);
        glDeleteShader(shader->handle);
        return -1;
    }

    return 0;
}

void gl_shader_destroy(const GlShader *shader) { glDeleteShader(shader->handle); }

int gl_shader_program_init(GlShaderProgram *shader_program) {
    shader_program->handle = glCreateProgram();
    if (shader_program->handle == 0) {
        fprintf(stderr, "[ERROR][SHADER] Failed to create shader program\n");
        return -1;
    }
    return 0;
}

void gl_shader_program_attach(const GlShaderProgram *shader_program, const GlShader *shader) {
    glAttachShader(shader_program->handle, shader->handle);
}

void gl_shader_program_bind(const GlShaderProgram *shader_program) {
    glUseProgram(shader_program->handle);
}

int gl_shader_program_link(const GlShaderProgram *shader_program) {
    glLinkProgram(shader_program->handle);

    int success;
    glGetProgramiv(shader_program->handle, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader_program->handle, sizeof(info_log), NULL, info_log);
        fprintf(stderr, "[ERROR][SHADER] Program linking failed\n%s\n", info_log);
        return -1;
    }

    return 0;
}

void gl_shader_program_destroy(GlShaderProgram *shader_program) {
    glDeleteProgram(shader_program->handle);
    shader_program->handle = 0;
}
