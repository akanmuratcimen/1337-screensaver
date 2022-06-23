#include <GL/glew.h>
#include <cglm/cglm.h>

#include "draw.h"
#include "shader_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shaders/char_vs.h"
#include "shaders/char_fs.h"

#include "shaders/rect_vs.h"
#include "shaders/rect_fs.h"

static bool rect_shader_loaded = false;
GLuint rect_shader_id;

void
draw_rectangle(
  const float x,
  const float y,
  const float w,
  const float h,
  struct window_size window_size
) {
  if (!rect_shader_loaded) {
    rect_shader_id = compile_shaders(
      shaders_rect_vs_glsl,
      shaders_rect_fs_glsl
    );

    rect_shader_loaded = true;
  }

  glUseProgram(rect_shader_id);

  mat4 transform;
  glm_mat4_identity(transform);

  glm_ortho(
    0.0f,
    window_size.width,
    window_size.height,
    0.0f,
    -1.0f,
    1.0f,
    transform
  );

  glUniformMatrix4fv(1, 1, GL_FALSE, transform[0]);

  float vertices[] = {
    x, y,
    x + w, y + h,
    x, y + h,

    x, y,
    x + w, y + h,
    x + w, y
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
    2 * sizeof(float),
    (void *) 0
  );

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
}

#include "textures/sprite_1k.h"
#include "textures/sprite_2k.h"
#include "textures/sprite_4k.h"

struct texture
load_texture(
  const char *texture_name
) {
  unsigned char *texture_data = NULL;
  unsigned int texture_data_len = 0;

  if (strcmp(texture_name, "1k") == 0) {
    texture_data = textures_sprite_1k_png;
    texture_data_len = textures_sprite_1k_png_len;
  } else if (strcmp(texture_name, "2k") == 0) {
    texture_data = textures_sprite_2k_png;
    texture_data_len = textures_sprite_2k_png_len;
  } else if (strcmp(texture_name, "4k") == 0) {
    texture_data = textures_sprite_4k_png;
    texture_data_len = textures_sprite_4k_png_len;
  }

  int width;
  int height;
  int channels;

  unsigned char *data =
    stbi_load_from_memory(
      texture_data,
      texture_data_len,
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
    GL_RGBA,
    width,
    height,
    0,
    GL_RGBA,
    GL_UNSIGNED_BYTE,
    data
  );

  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

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

static bool char_shader_loaded = false;
GLuint char_shader_id;

void
draw_bulk(
  const int column_count,
  const int row_count,
  struct cell cells[column_count][row_count],
  struct window_size window_size,
  const float cell_size,
  const char *texture_file_name
) {
  if (!texture_loaded) {
    texture = load_texture(texture_file_name);
    texture_loaded = true;
  }

  if (!char_shader_loaded) {
    char_shader_id = compile_shaders(
      shaders_char_vs_glsl,
      shaders_char_fs_glsl
    );

    char_shader_loaded = true;
  }

  glUseProgram(char_shader_id);

  mat4 transform;
  glm_mat4_identity(transform);

  glm_ortho(
    0.0f,
    window_size.width,
    window_size.height,
    0.0f,
    -1.0f,
    1.0f,
    transform
  );

  glUniformMatrix4fv(4, 1, GL_FALSE, transform[0]);

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
    (void *) 0
  );

  glEnableVertexAttribArray(2);

  glVertexAttribPointer(
    2,
    2,
    GL_FLOAT,
    GL_FALSE,
    4 * sizeof(float),
    (void *) (2 * sizeof(float))
  );

  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, iVBO);

  glVertexAttribPointer(
    1,
    2,
    GL_FLOAT,
    GL_FALSE,
    4 * sizeof(float),
    (void *) 0
  );

  glVertexAttribDivisor(1, 1);

  glEnableVertexAttribArray(3);

  glVertexAttribPointer(
    3,
    2,
    GL_FLOAT,
    GL_FALSE,
    4 * sizeof(float),
    (void *) (2 * sizeof(float))
  );

  glVertexAttribDivisor(3, 1);

  glBindVertexArray(VAO);
  glDrawArraysInstanced(GL_TRIANGLES, 0, 6, column_count * row_count);
  glBindVertexArray(0);

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &iVBO);
}

