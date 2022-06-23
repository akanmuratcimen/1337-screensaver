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
  unsigned char *buffer,
  const GLenum shaderType
) {
  int compilationStatus;

  GLuint shader = glCreateShader(shaderType);

  glShaderSource(shader, 1, (const char **) &buffer, NULL);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compilationStatus);

  if (compilationStatus != GL_TRUE) {
    fprintf(stderr, "VERTEX SHADER ERROR\n");
    show_gl_shader_compilation_error(shader);
  }

  return shader;
}

GLuint
compile_shaders(
  unsigned char *vs_file_buffer,
  unsigned char *fs_file_buffer
) {
  GLuint program = glCreateProgram();

  GLuint vs = load_shader(vs_file_buffer, GL_VERTEX_SHADER);
  GLuint fs = load_shader(fs_file_buffer, GL_FRAGMENT_SHADER);

  glAttachShader(program, vs);
  glAttachShader(program, fs);

  glLinkProgram(program);

  glDeleteShader(vs);
  glDeleteShader(fs);

  return program;
}

#endif
