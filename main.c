#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include <GL/glew.h>
#include <cglm/cglm.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shaders/char_vs.h"
#include "shaders/char_fs.h"

#include "shaders/rect_vs.h"
#include "shaders/rect_fs.h"

#define BG_COLOR 12.0f/255.0f, 12.0f/255.0f, 245.0f/255.0f, 1.0f

#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)

#if defined(__cplusplus)
extern "C" {
#endif

struct texture {
  unsigned int id;
  int width;
  int height;
};

struct texture_offset {
  float x;
  float width;
};

struct vector2 {
  float x;
  float y;
};

struct texture
load_texture(
  const char *file_name
);

struct cell {
  char value;
  int column;
  int row;
  struct vector2 position;
  float pause_position;
  bool is_highlighting;
  bool is_highlighted;
};

struct column {
  float speed;
  bool is_highlighting;
  float highlighting_time_start;
  float top_position;
  int highlighting_row_index;
};

const char chars[] = { '1', '3', '7' };


struct window_size {
  int width;
  int height;
} window_size;

float line_height;
float match_threshold;
float is_aligned_threshold;
float aligned_speed;
float screen_offset_x;
float char_width;
char *texture_name;
int speed;

void
create_window(
  void
);

bool
loop(
  void
);

void
swap_buffers(
  void
);

void
terminate(
  void
);

void
set_initial_mouse_position(
  void
);

void
init_scaling(
  const int screen_width
);

void
draw_rectangle(
  const float x,
  const float y,
  const float w,
  const float h,
  struct window_size window_size
);

void
draw_bulk(
  const int column_count,
  const int row_count,
  struct cell cells[column_count][row_count],
  struct window_size window_size,
  const float cell_size,
  const char *texture_name
);

int
get_random_number_int(
  const int min,
  const int max
) {
  return (rand() % (abs(max - min) + 1) + min);
}

float
get_random_number_float(
  const float min,
  const float max
) {
  return min + (rand() / (float) RAND_MAX) * (max - min);
}

void
initialize_columns(
  const int column_count,
  struct column columns[column_count],
  bool first_time
) {
  for (int ci = 0; ci < column_count; ci++) {
    columns[ci].speed = get_random_number_float(speed, speed * 2.0f);
    columns[ci].top_position = 9999.9f;

    if (first_time) {
      columns[ci].is_highlighting = false;
      columns[ci].highlighting_row_index = -1;
      columns[ci].highlighting_time_start = -1.0f;
    }
  }
}

void
generate_random_cells(
  const int column_count,
  const int row_count,
  struct cell cells[column_count][row_count]
) {
  for (int ci = 0; ci < column_count; ci++) {
    for (int ri = 0; ri < row_count; ri++) {
      const struct vector2 position = {
        ci * char_width + screen_offset_x,
        ri * line_height - window_size.height
      };

      struct cell cell;

      cell.value = chars[get_random_number_int(0, 2)];
      cell.column = ci;
      cell.row = ri;
      cell.position = position;
      cell.is_highlighting = false;
      cell.is_highlighted = false;
      cell.pause_position = -1.0f;

      cells[ci][ri] = cell;
    }
  }
}

bool
is_cell_available_to_highlight(
  const struct column *columns,
  const struct cell cell
) {
  if (columns[cell.column].is_highlighting) {
    return false;
  }

  if (cell.is_highlighted) {
    return false;
  }

  if (cell.position.y < 0) {
    return false;
  }

  if (cell.position.y > window_size.height) {
    return false;
  }

  if (cell.position.x > window_size.width) {
    return false;
  }

  return true;
}

bool
is_cells_match(
  const struct column *columns,
  const struct cell source,
  const struct cell target,
  const char expected_char
) {
  if (target.value != expected_char) {
    return false;
  }

  if (!is_cell_available_to_highlight(columns, target)) {
    return false;
  }

  if (fabsf(target.position.y - source.position.y) > match_threshold) {
    return false;
  }

  return true;
}

float
get_time(
  void
) {
  struct timespec ts = { 0 };

  clock_gettime(CLOCK_MONOTONIC, &ts);

  unsigned long long int time =
    (unsigned long long int)ts.tv_sec * 1000000000LLU +
    (unsigned long long int)ts.tv_nsec;

  return (float)(time) * 1e-9;
}

int
main(
#if defined(_WIN32)
  int argc,
  char* argv[]
#else
  void
#endif
) {
  create_window();
  init_scaling(window_size.width);

  srand(get_time());

  int column_count = ceil(window_size.width / char_width);
  int row_count = ceil(window_size.height / char_width) * 2;

  struct column columns[column_count];
  struct cell cells[column_count][row_count];

  initialize_columns(column_count, columns, true);
  generate_random_cells(column_count, row_count, cells);

  float refresh_columns_timer = get_time();
  float initialization_timer = get_time();

  bool warmup = false;

  float previous_time = get_time();
  float current_time = 0.0f;
  float delta_time = 0.0f;

  while (loop()) {
    current_time = get_time();
    delta_time = current_time - previous_time;
    previous_time = current_time;

    if (!warmup && get_time() - initialization_timer > 2.0f) {
      warmup = true;
      set_initial_mouse_position();
    }

    if (!warmup) {
      goto draw;
    }

    for (int ci = 0; ci < column_count - 3; ci++) {
      if (columns[ci].is_highlighting) {
        continue;
      }

      for (int r0i = 0; r0i < row_count; r0i++) {
        if (cells[ci][r0i].value != '1') {
          continue;
        }

        if (
          !is_cell_available_to_highlight(
            columns,
            cells[ci][r0i]
          )
        ) {
          continue;
        }

        int r1i = 0;
        for (; r1i < row_count; r1i++) {
          if (
            is_cells_match(
              columns,
              cells[ci][r0i],
              cells[ci + 1][r1i],
              '3'
            )
          ) {
            break;
          }
        }

        if (r1i == row_count) {
          continue;
        }

        int r2i = 0;
        for (; r2i < row_count; r2i++) {
          if (
            is_cells_match(
              columns,
              cells[ci][r0i],
              cells[ci + 2][r2i],
              '3'
            )
          ) {
            break;
          }
        }

        if (r2i == row_count) {
          continue;
        }

        int r3i = 0;
        for (; r3i < row_count; r3i++) {
          if (
            is_cells_match(
              columns,
              cells[ci][r0i],
              cells[ci + 3][r3i],
              '7'
            )
          ) {
            break;
          }
        }

        if (r3i == row_count) {
          continue;
        }

        float maxY = MAX(
          cells[ci + 0][r0i].position.y, MAX(
            cells[ci + 1][r1i].position.y, MAX(
              cells[ci + 2][r2i].position.y,
              cells[ci + 3][r3i].position.y
            )
          )
        );

        for (int ri = 0; ri < row_count; ri++) {
          cells[ci + 0][ri].pause_position =
            cells[ci + 0][ri].position.y + maxY -
              cells[ci + 0][r0i].position.y;
        }

        for (int ri = 0; ri < row_count; ri++) {
          cells[ci + 1][ri].pause_position =
            cells[ci + 1][ri].position.y + maxY -
              cells[ci + 1][r1i].position.y;
        }

        for (int ri = 0; ri < row_count; ri++) {
          cells[ci + 2][ri].pause_position =
            cells[ci + 2][ri].position.y + maxY -
              cells[ci + 2][r2i].position.y;
        }

        for (int ri = 0; ri < row_count; ri++) {
          cells[ci + 3][ri].pause_position =
            cells[ci + 3][ri].position.y + maxY -
              cells[ci + 3][r3i].position.y;
        }

        cells[ci + 0][r0i].is_highlighting = true;
        cells[ci + 1][r1i].is_highlighting = true;
        cells[ci + 2][r2i].is_highlighting = true;
        cells[ci + 3][r3i].is_highlighting = true;

        columns[ci + 0].is_highlighting = true;
        columns[ci + 1].is_highlighting = true;
        columns[ci + 2].is_highlighting = true;
        columns[ci + 3].is_highlighting = true;

        float time = get_time();

        columns[ci + 0].highlighting_time_start = time;
        columns[ci + 1].highlighting_time_start = time;
        columns[ci + 2].highlighting_time_start = time;
        columns[ci + 3].highlighting_time_start = time;

        columns[ci + 0].highlighting_row_index = r0i;
        columns[ci + 1].highlighting_row_index = r1i;
        columns[ci + 2].highlighting_row_index = r2i;
        columns[ci + 3].highlighting_row_index = r3i;
      }
    }

  draw:
    glClearColor(BG_COLOR);
    glClear(GL_COLOR_BUFFER_BIT);

    for (int ci = 0; ci < column_count - 3; ci++) {
      struct column column0 = columns[ci + 0];
      struct column column1 = columns[ci + 1];
      struct column column2 = columns[ci + 2];
      struct column column3 = columns[ci + 3];

      if (
        column0.is_highlighting &&
        column1.is_highlighting &&
        column2.is_highlighting &&
        column3.is_highlighting
      ) {
        if (
          fabsf(
            cells[ci + 0][column0.highlighting_row_index].position.y -
            cells[ci + 1][column1.highlighting_row_index].position.y
          ) <= is_aligned_threshold &&

          fabsf(
            cells[ci + 1][column1.highlighting_row_index].position.y -
            cells[ci + 2][column2.highlighting_row_index].position.y
          ) <= is_aligned_threshold &&

          fabsf(
            cells[ci + 2][column2.highlighting_row_index].position.y -
            cells[ci + 3][column3.highlighting_row_index].position.y
          ) <= is_aligned_threshold
        ) {
          for (int ri = 0; ri < row_count; ri++) {
            cells[ci + 0][ri].position.y += aligned_speed * delta_time;
            cells[ci + 1][ri].position.y += aligned_speed * delta_time;
            cells[ci + 2][ri].position.y += aligned_speed * delta_time;
            cells[ci + 3][ri].position.y += aligned_speed * delta_time;
          }
        }
      }
    }

    for (int ci = 0; ci < column_count; ci++) {
      if (columns[ci].is_highlighting) {
        if (get_time() - columns[ci].highlighting_time_start > 2.4f) {
          cells[ci][columns[ci].highlighting_row_index].is_highlighted = true;
          cells[ci][columns[ci].highlighting_row_index].pause_position = -1.0f;

          columns[ci].highlighting_time_start = -1.0f;
          columns[ci].is_highlighting = false;
          columns[ci].highlighting_row_index = -1;
        }
      }

      for (int ri = 0; ri < row_count; ri++) {
        struct cell cell = cells[ci][ri];

        if (
          columns[ci].is_highlighting &&
          cell.position.y >= cell.pause_position
        ) {
          if (cell.is_highlighting && !cell.is_highlighted) {
            draw_rectangle(
              cell.position.x,
              cell.position.y - (char_width * 0.1f),
              char_width,
              char_width * 1.2f,
              window_size
            );
          };
        } else {
          cells[ci][ri].position.y += columns[ci].speed * delta_time;
        }
      }
    }

    draw_bulk(
      column_count,
      row_count,
      cells,
      window_size,
      char_width,
      texture_name
    );

    if (get_time() - refresh_columns_timer >= 4.0f) {
      refresh_columns_timer = get_time();

      initialize_columns(column_count, columns, false);

      for (int ci = 0; ci < column_count; ci++) {
        if (columns[ci].is_highlighting) {
          continue;
        }

        for (int ri = 0; ri < row_count; ri++) {
          columns[ci].top_position = MIN(
            columns[ci].top_position,
            cells[ci][ri].position.y
          );
        }

        for (int ri = 0; ri < row_count; ri++) {
          if (cells[ci][ri].position.y <= window_size.height) {
            continue;
          }

          columns[ci].top_position = columns[ci].top_position - line_height;

          cells[ci][ri].is_highlighted = false;
          cells[ci][ri].is_highlighting = false;
          cells[ci][ri].position.y = columns[ci].top_position;
          cells[ci][ri].value = chars[get_random_number_int(0, 2)];
        }
      }
    }

    glFlush();
    swap_buffers();
  }

  terminate();

  return 0;
}

void
init_scaling(
  const int screen_width
) {
  if (screen_width <= 1920) {
    char_width = 110.0f;
    texture_name = "1k";
    speed = 70;
  } else if (screen_width <= 2560) {
    char_width = 150.0f;
    texture_name = "2k";
    speed = 100;
  } else {
    char_width = 230.0f;
    texture_name = "4k";
    speed = 120;
  }

  aligned_speed = speed * 1.0f;
  line_height = char_width * 1.25f;
  match_threshold = char_width * 0.5f;
  is_aligned_threshold = char_width * 0.1f;
  screen_offset_x = char_width * 0.4f * -1.0f;
}

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

#include <GLFW/glfw3.h>

#define FULLSCREEN 1
#define EXIT_ON_INPUT 0

GLFWwindow *window = NULL;

bool is_any_key_pressed = false;
bool is_any_mouse_button_pressed = false;
struct vector2 initial_mouse_position;
bool is_initial_mouse_position_set = false;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void
key_callback(
  GLFWwindow* window,
  int key,
  int scancode,
  int action,
  int mods
) {
  is_any_key_pressed = true;
}

void
mouse_button_callback(
  GLFWwindow* window,
  int button,
  int action,
  int mods
) {
  is_any_mouse_button_pressed = true;
}

#pragma GCC diagnostic pop

struct vector2
get_cursor_position(
  void
) {
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  return (struct vector2) { xpos, ypos };
}

void
create_window(
  void
) {
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);

  window =
    glfwCreateWindow(
      mode->width,
      mode->height,
      "1337 Screensaver",
      FULLSCREEN ? monitor : NULL,
      NULL
    );

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

  glfwSwapInterval(1);
  glewInit();

  glViewport(0, 0, mode->width, mode->height);

  int width, height;
  glfwGetWindowSize(window, &width, &height);

  window_size = (struct window_size) { width, height };
}

bool
is_mouse_moved(
  const struct vector2 initial_position
) {
  if (!is_initial_mouse_position_set) {
    return false;
  }

  const struct vector2 current_position = get_cursor_position();

  return
    current_position.x != initial_position.x ||
    current_position.y != initial_position.y;
}

bool
is_interrupted_by_input(
  void
) {
#if EXIT_ON_INPUT == 1
  return
    is_any_mouse_button_pressed ||
    is_any_key_pressed ||
    is_mouse_moved(initial_mouse_position);
#else
  return false;
#endif
}

bool
loop(
  void
) {
  if (glfwWindowShouldClose(window)) {
    return false;
  }

  if (is_interrupted_by_input()) {
    return false;
  }

  return true;
}

void
swap_buffers(
  void
) {
  glfwSwapBuffers(window);
  glfwPollEvents();
}

void
terminate(
  void
) {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void
set_initial_mouse_position(
  void
) {
  initial_mouse_position = get_cursor_position();
  is_initial_mouse_position_set = true;
}

#if defined(__cplusplus)
}
#endif
