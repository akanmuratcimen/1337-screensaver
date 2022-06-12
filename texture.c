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

  glVertexAttribPointer(
    2,
    2,
    GL_FLOAT,
    GL_FALSE,
    8 * sizeof(float),
    (void *) (6 * sizeof(float))
  );

  glEnableVertexAttribArray(2);

  struct texture texture = {
    .id = id,
    .width = width,
    .height = height,
  };

  stbi_image_free(data);

  return texture;
}

struct texture_offset
get_texture_offset(
  const char c
) {
  struct texture_offset texture_offset;

  switch (c) {
    case '1':
      texture_offset.x = 0.0f;
      texture_offset.width = 57.0f;
      break;
    case '3':
      texture_offset.x = 72.0f;
      texture_offset.width = 113.0f;
      break;
    case '7':
      texture_offset.x = 188.0f;
      texture_offset.width = 112.0f;
      break;
  }

  return texture_offset;
}

void
draw_texture(
  struct texture texture,
  const char c,
  float x,
  float y
) {
  struct texture_offset texture_offset = get_texture_offset(c);

  float vertices[] = {
    // bottom right
    x + texture_offset.width, y + texture.height,
    (texture_offset.x + texture_offset.width) / texture.width, 1.0f,

    // top right
    x + texture_offset.width, y,
    (texture_offset.x + texture_offset.width) / texture.width, 0.0f,

    // top left
    x, y,
    texture_offset.x / texture.width, 0.0f,

    // bottom left
    x, y + texture.height,
    texture_offset.x / texture.width, 1.0f
  };

  unsigned int indices[] = {
    0, 1, 2,
    2, 3, 0
  };

  unsigned int VAO, VBO, EBO;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(vertices),
    vertices,
    GL_STATIC_DRAW
  );

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    sizeof(indices),
    indices,
    GL_STATIC_DRAW
  );

  glVertexAttribPointer(
    0,
    2,
    GL_FLOAT,
    GL_FALSE,
    4 * sizeof(float),
    (void *) 0
  );

  glEnableVertexAttribArray(0);

  glVertexAttribPointer(
    1,
    2,
    GL_FLOAT,
    GL_FALSE,
    4 * sizeof(float),
    (void *) (2 * sizeof(float))
  );

  glEnableVertexAttribArray(1);

  glBindTexture(GL_TEXTURE_2D, texture.id);
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

struct texture texture;
static bool texture_loaded = false;

void
draw_bulk(
  int column_count,
  int row_count,
  struct cell cells[column_count][row_count]
) {
  if (!texture_loaded) {
    texture = load_texture("sprite.jpg");
    texture_loaded = true;
  }

  for (int column = 0; column < column_count; column++) {
    for (int row = 0; row < row_count; row++) {
      struct cell cell = cells[column][row];

      draw_texture(
        texture,
        cell.value,
        cell.position.x,
        cell.position.y
      );
    }
  }
}

