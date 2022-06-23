#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "core.h"
#include "texture.h"

#define FULLSCREEN 0
#define FPS_LIMIT 30
#define WINDOW_TITLE "1337 Screensaver"

#define BG_COLOR 12.0f/255.0f, 12.0f/255.0f, 245.0f/255.0f, 1.0f
#define FONT_COLOR 5.0f/255.0f, 226.0f/255.0f, 115.0f/255.0f, 1.0f


#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)

#if defined(__cplusplus)
extern "C" {
#endif

const char chars[] = { '1', '3', '7' };

float line_height;
float match_threshold;
float is_aligned_threshold;
float aligned_speed;
float screen_offset_x;
float char_width;
int speed;

struct window_size window_size;

GLFWwindow *window = NULL;

bool is_any_key_pressed = false;
bool is_any_mouse_button_pressed = false;

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

void
create_window(
  void
) {
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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

  glfwSwapInterval(1);
  glewInit();

  glViewport(0, 0, mode->width, mode->height);

  int width, height;
  glfwGetWindowSize(window, &width, &height);

  window_size = (struct window_size) { width, height };
}

void
init_scaling(
  const int screen_width
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

struct vector2
get_cursor_position(
  void
) {
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  return (struct vector2) { xpos, ypos };
}

bool
is_mouse_moved(
  const struct vector2 initial_position
) {
  const struct vector2 current_position = get_cursor_position();

  return
    current_position.x != initial_position.x ||
    current_position.y != initial_position.y;
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
#if defined(_WIN32)

  if (argc < 2) {
    goto exit;
  }

  if (strcmp(argv[1], "/c") == 0) {
    goto exit;
  }

  if (strcmp(argv[1], "/p") == 0) {
    goto exit;
  }

  if (strcmp(argv[1], "/s") != 0) {
    goto exit;
  }

#endif

  create_window();
  init_scaling(window_size.width);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

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
  struct vector2 initial_mouse_position;

  float previous_time = get_time();
  float current_time = 0.0f;
  float delta_time = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    current_time = get_time();
    delta_time = current_time - previous_time;
    previous_time = current_time;

    if (!warmup && get_time() - initialization_timer > 3.0f) {
      warmup = true;
      initial_mouse_position = get_cursor_position();
    }

    if (!warmup) {
      goto draw;
    }

    if (
      is_any_mouse_button_pressed ||
      is_any_key_pressed ||
      is_mouse_moved(initial_mouse_position)
    ) {
      goto exit;
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
        if (get_time() - columns[ci].highlighting_time_start > 2.0f) {
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

    draw_bulk(column_count, row_count, cells, window_size, char_width);

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

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

exit:
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

void
init_scaling(
  const int screen_width
) {
  if (screen_width <= 1280) {
    char_width = 150.0f;
  } else if (screen_width <= 1440) {
    char_width = 150.0f;
  } else if (screen_width <= 2048) {
    char_width = 150.0f;
  } else if (screen_width <= 2560) {
    char_width = 150.0f;
  } else {
    char_width = 150.0f;
  }

  speed = 100;
  aligned_speed = speed * 1.0f;
  line_height = char_width * 1.25f;
  match_threshold = char_width * 0.45f;
  is_aligned_threshold = char_width * 0.1f;
  screen_offset_x = char_width * 0.4f * -1.0f;
}

#if defined(__cplusplus)
}
#endif
