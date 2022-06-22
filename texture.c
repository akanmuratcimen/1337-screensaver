#include <GL/glew.h>

#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct texture
load_texture(
  const char *file_name
) {
  int width;
  int height;
  int channels;

  unsigned char *data =
    stbi_load(
      file_name,
      &width,
      &height,
      &channels,
      0
    );

  unsigned int id;

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RGB,
    width,
    height,
    0,
    GL_RGB,
    GL_UNSIGNED_BYTE,
    data
  );

  glGenerateMipmap(GL_TEXTURE_2D);

  struct texture texture = {
    .id = id,
    .width = width,
    .height = height,
  };

  stbi_image_free(data);

  return texture;
}

struct texture texture;
static bool texture_loaded = false;

void
draw_bulk(
  const int column_count,
  const int row_count,
  struct cell cells[column_count][row_count]
) {
  if (!texture_loaded) {
    texture = load_texture("sprite.jpg");
    texture_loaded = true;
  }

  float cell_size = 150.0f;
  float v[column_count * row_count][4];

  int i = 0;
  for (int column = 0; column < column_count; column++) {
    for (int row = 0; row < row_count; row++, i++) {
      struct cell cell = cells[column][row];

      v[i][0] = cell.position.x;
      v[i][1] = cell.position.y;

      int tex_mul = 0;

      switch (cell.value) {
        case '1': tex_mul = 0; break;
        case '3': tex_mul = 1; break;
        case '7': tex_mul = 2; break;
      }

      v[i][2] = tex_mul * cell_size / texture.width;
      v[i][3] = 0.0f;
    }
  }

  unsigned int iVBO;

  glGenBuffers(1, &iVBO);
  glBindBuffer(GL_ARRAY_BUFFER, iVBO);

  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(float) * 4 * column_count * row_count,
    &v,
    GL_STATIC_DRAW
  );

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  float tx = cell_size / texture.width;
  float ty = cell_size / texture.height;

  float vertices[] = {
    0.0f, cell_size,       0.0f, ty,   // bottom left
    cell_size, 0.0f,       tx, 0.0f,   // top right
    0.0f, 0.0f,            0.0f, 0.0f, // top left

    0.0f, cell_size,       0.0f, ty,   // bottom left
    cell_size, 0.0f,       tx, 0.0f,   // top right
    cell_size, cell_size,  tx, ty      // bottom right
  };

  unsigned int VAO, VBO;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(vertices),
    vertices,
    GL_STATIC_DRAW
  );

  glEnableVertexAttribArray(0);

  glVertexAttribPointer(
    0,
    2,
    GL_FLOAT,
    GL_FALSE,
    4 * sizeof(float),
    (void *)0
  );

  glEnableVertexAttribArray(2);

  glVertexAttribPointer(
    2,
    2,
    GL_FLOAT,
    GL_FALSE,
    4 * sizeof(float),
    (void *)(2 * sizeof(float))
  );

  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, iVBO);

  glVertexAttribPointer(
    1,
    2,
    GL_FLOAT,
    GL_FALSE,
    4 * sizeof(float),
    (void *)0
  );

  glVertexAttribDivisor(1, 1);

  glEnableVertexAttribArray(3);

  glVertexAttribPointer(
    3,
    2,
    GL_FLOAT,
    GL_FALSE,
    4 * sizeof(float),
    (void *)(2 * sizeof(float))
  );

  glVertexAttribDivisor(3, 1);

  glBindVertexArray(VAO);
  glDrawArraysInstanced(GL_TRIANGLES, 0, 6, column_count * row_count);
  glBindVertexArray(0);
}

