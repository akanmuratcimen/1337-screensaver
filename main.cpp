#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#define BACK_R 12
#define BACK_G 12
#define BACK_B 245

#define FORE_R 5
#define FORE_G 226
#define FORE_B 115

#define FONT_SIZE 96

GLuint
compile_shaders(
  void
);

struct Character {
  GLuint TextureID;
  glm::ivec2 Size;
  glm::ivec2 Bearing;
  GLuint Advance;
};

std::map<GLchar, Character> characters;

void
setup_characters(
  void
) {
  FT_Library ft;
  FT_Init_FreeType(&ft);
  FT_Face face;

  FT_New_Face(ft, "sora.ttf", 0, &face);
  FT_Set_Pixel_Sizes(face, 0, FONT_SIZE);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  for (auto c : { '1', '3', '7' }) {
    FT_Load_Char(face, c, FT_LOAD_RENDER);
    GLuint texture;

    glCreateTextures(GL_TEXTURE_2D, 1, &texture);

    glTextureStorage2D(
      texture,
      1,
      GL_R8,
      face->glyph->bitmap.width,
      face->glyph->bitmap.rows
    );

    glTextureSubImage2D(
      texture,
      0,
      0,
      0,
      face->glyph->bitmap.width,
      face->glyph->bitmap.rows,
      GL_RED,
      GL_UNSIGNED_BYTE,
      face->glyph->bitmap.buffer
    );

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    Character character = {
      texture,
      glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
      glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
      (unsigned int) face->glyph->advance.x
    };

    characters.insert(std::pair<GLchar, Character>(c, character));
  }

  FT_Done_Face(face);
  FT_Done_FreeType(ft);
}

Character
render_character(
  const char c,
  GLuint *buffer,
  const float x,
  const float y
) {
  Character ch = characters[c];

  GLfloat xpos = x + ch.Bearing.x;
  GLfloat ypos = y - (ch.Size.y - ch.Bearing.y);
  GLfloat w = ch.Size.x;
  GLfloat h = ch.Size.y;

  GLfloat vertices[6 * 4] = {
    xpos, ypos + h, 0.0f, 0.0f,
    xpos, ypos, 0.0f, 1.0f,
    xpos + w, ypos, 1.0f, 1.0f,
    xpos, ypos + h, 0.0f, 0.0f,
    xpos + w, ypos, 1.0f, 1.0f,
    xpos + w, ypos + h, 1.0f, 0.0f
  };

  glNamedBufferSubData(*buffer, 0, sizeof(GLfloat) * 6 * 4, vertices);
  glBindTexture(GL_TEXTURE_2D, ch.TextureID);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  return ch;
}

int
main(
  int argc,
  char *argv[]
) {
  glfwInit();

  GLFWmonitor* monitor =  glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);

  float width = mode->width;
  float height = mode->height;

  GLFWwindow *window = glfwCreateWindow(
    width,
    height,
    "1337 Screensaver",
    monitor,
    NULL
  );

  glViewport(0, 0, width, height);

  glfwMakeContextCurrent(window);

  glewInit();
  glEnable(GL_CULL_FACE);
  glClearColor(BACK_R / 255.0f, BACK_G / 255.0f, BACK_B / 255.0f, 1.0f);

  GLuint shader = compile_shaders();
  glUseProgram(shader);

  setup_characters();

  glm::mat4 projection =
    glm::ortho(
      0.0f,
      mode->width * 1.0f,
      0.0f,
      mode->height * 1.0f
    );

  glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(projection));

  GLuint vao;

  glCreateVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint buffer;

  glCreateBuffers(1, &buffer);

  glNamedBufferStorage(buffer, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_STORAGE_BIT);
  glVertexArrayVertexBuffer(vao, 0, buffer, 0, sizeof(GLfloat) * 4);
  glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribBinding(vao, 0, 0);
  glEnableVertexArrayAttrib(vao, 0);

  glUniform3f(7, FORE_R / 255.0f, FORE_G / 255.0f, FORE_B / 255.0f);
  glUniform3f(8, BACK_R / 255.0f, BACK_G / 255.0f, BACK_B / 255.0f);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    GLfloat y = -42;

    int columns = ceil(width / (FONT_SIZE / 2 * 4)) + 12;
    int rows = ceil(height / FONT_SIZE) + 3;

    for (auto i = 1; i <= rows; ++i) {
      GLfloat x = (-FONT_SIZE * 3) + (i * FONT_SIZE / 2) - FONT_SIZE * 10;

      for (auto j = 0; j <= columns; ++j) {
        for (auto c : { '1', '3', '3', '7' }) {
          auto ch = render_character(c, &buffer, x, y);
          x += ch.Advance >> 6;
        }
      }

      y += 96;
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}

void
show_gl_shader_compilation_error(
  const GLuint shaderHandle
) {
  int errorLogLength;

  glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &errorLogLength);
  char *buffer = new char[errorLogLength + 1];

  glGetShaderInfoLog(shaderHandle, errorLogLength + 1, NULL, buffer);
  fprintf(stderr, buffer);

  delete buffer;
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
  char *buffer = new char[length + 1];

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

  delete buffer;

  return shader;
}

GLuint
compile_shaders(
  void
) {
  GLuint program = glCreateProgram();

  GLuint vs = load_shader("./shaders/vs.glsl", GL_VERTEX_SHADER);
  GLuint fs = load_shader("./shaders/fs.glsl", GL_FRAGMENT_SHADER);

  glAttachShader(program, vs);
  glAttachShader(program, fs);

  glLinkProgram(program);

  glDeleteShader(vs);
  glDeleteShader(fs);

  return program;
}

