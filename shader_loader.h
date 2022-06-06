#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <stdio.h>

void
show_gl_shader_compilation_error(
  const GLuint shaderHandle
) {
  int errorLogLength;

  glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &errorLogLength);
  char *buffer = (char *) malloc(errorLogLength + 1);

  glGetShaderInfoLog(shaderHandle, errorLogLength + 1, NULL, buffer);
  fprintf(stderr, buffer);

  free(buffer);
}

GLuint
load_shader(
  const char *path,
  const GLenum shaderType
) {
  FILE *f = fopen (path, "rb");

  if (!f) {
    return -1;
  }

  fseek(f, 0, SEEK_END);
  long length = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *buffer = (char *) malloc(length + 1);

  if (buffer) {
    fread(buffer, 1, length, f);
  }

  int compilationStatus;

  GLuint shader = glCreateShader(shaderType);

  glShaderSource(shader, 1, (const char **) &buffer, NULL);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compilationStatus);

  if (compilationStatus != GL_TRUE) {
    fprintf(stderr, "VERTEX SHADER ERROR\n");
    show_gl_shader_compilation_error(shader);
  }

  fclose(f);
  free(buffer);

  return shader;
}

GLuint
compile_shaders(
  const char *vs_file_path,
  const char *fs_file_path
) {
  GLuint program = glCreateProgram();

  GLuint vs = load_shader(vs_file_path, GL_VERTEX_SHADER);
  GLuint fs = load_shader(fs_file_path, GL_FRAGMENT_SHADER);

  glAttachShader(program, vs);
  glAttachShader(program, fs);

  glLinkProgram(program);

  glDeleteShader(vs);
  glDeleteShader(fs);

  return program;
}

#endif
