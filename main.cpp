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

int
main(
  int argc,
  char *argv[]
) {
  glfwInit();

  GLFWwindow * window = glfwCreateWindow(800, 600, "YEAH", NULL, NULL);

  glfwMakeContextCurrent(window);

  glewInit();
  glEnable(GL_CULL_FACE);
  glViewport(0, 0, 800, 600);
  glClearColor(BACK_R / 255.0f, BACK_G / 255.0f, BACK_B / 255.0f, 1.0f);

  GLuint shader = compile_shaders();
  glUseProgram(shader);

  FT_Library ft;
  FT_Init_FreeType(&ft);
  FT_Face face;

  FT_New_Face(ft, "arial.ttf", 0, &face);
  FT_Set_Pixel_Sizes(face, 0, 48);

  std::map<GLchar, Character> characters;

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

  glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
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

  std::string text("1337");

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    GLfloat x = 24.0f;
    GLfloat y = 200 - 72.0f;
    GLfloat scale = 1.0f;

    std::string::const_iterator c;

    for (c = text.begin(); c != text.end(); c++) {
      Character ch = characters[*c];

      GLfloat xpos = x + ch.Bearing.x * scale;
      GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
      GLfloat w = ch.Size.x * scale;
      GLfloat h = ch.Size.y * scale;

      GLfloat vertices[6 * 4] = {
        xpos, ypos + h, 0.0f, 0.0f,
        xpos, ypos, 0.0f, 1.0f,
        xpos + w, ypos, 1.0f, 1.0f,
        xpos, ypos + h, 0.0f, 0.0f,
        xpos + w, ypos, 1.0f, 1.0f,
        xpos + w, ypos + h, 1.0f, 0.0f
      };

      glNamedBufferSubData(buffer, 0, sizeof(GLfloat) * 6 * 4, vertices);
      glBindTexture(GL_TEXTURE_2D, ch.TextureID);
      glDrawArrays(GL_TRIANGLES, 0, 6);

      x += (ch.Advance >> 6) * scale;
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

