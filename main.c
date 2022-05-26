#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "raylib.h"
#include "font.h"

#define FULLSCREEN 1
#define FPS_LIMIT 30
#define FONT_FAMILY "sora.ttf"
#define WINDOW_TITLE "1337 Screensaver"

#define BG_COLOR CLITERAL(Color){ 12, 12, 245, 255 }
#define FONT_COLOR CLITERAL(Color){ 5, 226, 115, 255 }

#define COLUMN_COUNT 40
#define ROW_COUNT 40

#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)

#if defined(__cplusplus)
extern "C" {
#endif

struct cell {
  const char *value;
  int column;
  int row;
  Vector2 position;
  float pause_position;
  bool is_highlighting;
  bool is_highlighted;
};

struct column {
  float speed;
  bool is_highlighting;
  int highlighting_frame_counter;
  float top_position;
  int highlighting_row_index;
};

const char *chars[] = { "1", "3", "7" };

int font_size;
float line_height;
float match_threshold;
float is_aligned_threshold;
float aligned_speed;
float screen_offset_x;
float char_width;
int highlight_rect_width;
int highlight_rect_height;
int highlight_rect_offset_x;
int highlight_rect_offset_y;
int speed_min;
int speed_max;

struct cell cells[COLUMN_COUNT][ROW_COUNT];
struct column columns[COLUMN_COUNT];

void
init_scaling(
  const int screen_width
);

void
initialize_columns(
  void
) {
  for (int ci = 0; ci < COLUMN_COUNT; ci++) {
    columns[ci].speed = GetRandomValue(speed_min, speed_max);
    columns[ci].top_position = 9999.9f;
  }
}

void
generate_random_cells(
  void
) {
  for (int ci = 0; ci < COLUMN_COUNT; ci++) {
    for (int ri = 0; ri < ROW_COUNT; ri++) {
      const Vector2 position = {
        ci * char_width + screen_offset_x,
        ri * line_height - GetScreenHeight()
      };

      struct cell cell;

      cell.value = chars[GetRandomValue(0, 2)];
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

  if (cell.position.y > GetScreenHeight()) {
    return false;
  }

  if (cell.position.x > GetScreenWidth()) {
    return false;
  }

  return true;
}

bool
is_cells_match(
  const struct cell source,
  const struct cell target,
  const char expected_char
) {
  if (target.value[0] != expected_char) {
    return false;
  }

  if (!is_cell_available_to_highlight(target)) {
    return false;
  }

  if (fabsf(target.position.y - source.position.y) > match_threshold) {
    return false;
  }

  return true;
}

bool is_any_key_pressed(
  void
) {
  return GetKeyPressed() != 0;
}

bool is_mouse_moved_or_button_presses(
  const Vector2 initial_mouse_position
) {
    if (
      IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
      IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) ||
      IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)
    ) {
      return true;
    }

    const Vector2 current_mouse_position = GetMousePosition();

    if (
      current_mouse_position.x != initial_mouse_position.x ||
      current_mouse_position.y != initial_mouse_position.y
    ) {
      return true;
    }

    return false;
}

bool is_any_touch_detected(
  void
) {
  return GetTouchPointCount() != 0;
}

int main(
#if defined(_WIN32)
  int argc,
  char* argv[]
#else
  void
#endif
) {
  if (!FULLSCREEN) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  }

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

  InitWindow(GetScreenWidth(), GetScreenHeight(), WINDOW_TITLE);
  init_scaling(GetScreenWidth());

  const Font font =
    LoadFontFromMemory(
      ".ttf",
      sora_ttf,
      sora_ttf_len,
      font_size,
      0,
      0
    );

  if (FULLSCREEN) {
    ToggleFullscreen();
  }

  HideCursor();
  SetTargetFPS(FPS_LIMIT);

  initialize_columns();
  generate_random_cells();

  int refresh_columns_frame_counter = 0;
  int initialization_frame_counter = 0;
  bool warmup = false;
  Vector2 initial_mouse_position;

  while (!WindowShouldClose()) {
    refresh_columns_frame_counter++;

    if (!warmup) {
      if (initialization_frame_counter > 30) {
        warmup = true;
        initial_mouse_position = GetMousePosition();
      } else {
        initialization_frame_counter++;
      }
    }

    if (!warmup) {
      goto draw;
    }

    if (
      is_any_key_pressed() ||
      is_mouse_moved_or_button_presses(initial_mouse_position) ||
      is_any_touch_detected()
    ) {
      goto exit;
    }

    for (int ci = 0; ci < COLUMN_COUNT - 3; ci++) {
      if (columns[ci].is_highlighting) {
        continue;
      }

      for (int r0i = 0; r0i < ROW_COUNT; r0i++) {
        if (cells[ci][r0i].value[0] != '1') {
          continue;
        }

        if (!is_cell_available_to_highlight(cells[ci][r0i])) {
          continue;
        }

        int r1i = 0;
        for (; r1i < ROW_COUNT; r1i++) {
          if (is_cells_match(cells[ci][r0i], cells[ci + 1][r1i], '3')) {
            break;
          }
        }

        if (r1i == ROW_COUNT) {
          continue;
        }

        int r2i = 0;
        for (; r2i < ROW_COUNT; r2i++) {
          if (is_cells_match(cells[ci][r0i], cells[ci + 2][r2i], '3')) {
            break;
          }
        }

        if (r2i == ROW_COUNT) {
          continue;
        }

        int r3i = 0;
        for (; r3i < ROW_COUNT; r3i++) {
          if (is_cells_match(cells[ci][r0i], cells[ci + 3][r3i], '7')) {
            break;
          }
        }

        if (r3i == ROW_COUNT) {
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

        for (int ri = 0; ri < ROW_COUNT; ri++) {
          cells[ci + 0][ri].pause_position =
            cells[ci + 0][ri].position.y + maxY -
              cells[ci + 0][r0i].position.y;
        }

        for (int ri = 0; ri < ROW_COUNT; ri++) {
          cells[ci + 1][ri].pause_position =
            cells[ci + 1][ri].position.y + maxY -
              cells[ci + 1][r1i].position.y;
        }

        for (int ri = 0; ri < ROW_COUNT; ri++) {
          cells[ci + 2][ri].pause_position =
            cells[ci + 2][ri].position.y + maxY -
              cells[ci + 2][r2i].position.y;
        }

        for (int ri = 0; ri < ROW_COUNT; ri++) {
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

        columns[ci + 0].highlighting_row_index = r0i;
        columns[ci + 1].highlighting_row_index = r1i;
        columns[ci + 2].highlighting_row_index = r2i;
        columns[ci + 3].highlighting_row_index = r3i;
      }
    }

  draw:
    BeginDrawing();

      ClearBackground(BG_COLOR);

      for (int ci = 0; ci < COLUMN_COUNT - 3; ci++) {
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
            for (int ri = 0; ri < ROW_COUNT; ri++) {
              cells[ci + 0][ri].position.y += aligned_speed;
              cells[ci + 1][ri].position.y += aligned_speed;
              cells[ci + 2][ri].position.y += aligned_speed;
              cells[ci + 3][ri].position.y += aligned_speed;
            }
          }
        }
      }

      for (int ci = 0; ci < COLUMN_COUNT; ci++) {
        if (columns[ci].is_highlighting) {
          columns[ci].highlighting_frame_counter++;

          if (columns[ci].highlighting_frame_counter > 60) {
            cells[ci][columns[ci].highlighting_row_index].is_highlighted = true;
            cells[ci][columns[ci].highlighting_row_index].pause_position = -1.0f;

            columns[ci].highlighting_frame_counter = 0;
            columns[ci].is_highlighting = false;
            columns[ci].highlighting_row_index = -1;
          }
        }

        for (int ri = 0; ri < ROW_COUNT; ri++) {
          struct cell cell = cells[ci][ri];

          if (
            columns[ci].is_highlighting &&
            cell.position.y >= cell.pause_position
          ) {
            if (cell.is_highlighting && !cell.is_highlighted) {
              DrawRectangle(
                cell.position.x + highlight_rect_offset_x,
                cell.position.y + highlight_rect_offset_y,
                highlight_rect_width,
                highlight_rect_height,
                WHITE
              );
            };
          } else {
            cells[ci][ri].position.y += columns[ci].speed / 10.0f;
          }

          DrawTextEx(
            font,
            cell.value,
            cell.position,
            font_size,
            0,
            FONT_COLOR
          );
        }
      }

    EndDrawing();

    if (((refresh_columns_frame_counter / 100) % 2) == 1) {
      refresh_columns_frame_counter = 0;

      initialize_columns();

      for (int ci = 0; ci < COLUMN_COUNT; ci++) {
        if (columns[ci].is_highlighting) {
          continue;
        }

        for (int ri = 0; ri < ROW_COUNT; ri++) {
          columns[ci].top_position = MIN(
            columns[ci].top_position,
            cells[ci][ri].position.y
          );
        }

        for (int ri = 0; ri < ROW_COUNT; ri++) {
          if (cells[ci][ri].position.y > GetScreenHeight()) {
            columns[ci].top_position = columns[ci].top_position - line_height;

            cells[ci][ri].is_highlighted = false;
            cells[ci][ri].is_highlighting = false;
            cells[ci][ri].position.y = columns[ci].top_position;
            cells[ci][ri].value = chars[GetRandomValue(0, 2)];
          }
        }
      }
    }
  }

exit:

  UnloadFont(font);

  CloseWindow();

  return 0;
}

void
init_scaling(
  const int screen_width
) {
  if (screen_width <= 1280) {
    speed_min = 8;
    speed_max = speed_min * 2;
    aligned_speed = speed_min * 0.15f;
    font_size = 80;
    line_height = 70;
    match_threshold = 16.0f;
    is_aligned_threshold = 4.0f;
    screen_offset_x = -10.0f;
    char_width = 66.0f;
    highlight_rect_width = char_width;
    highlight_rect_height = 64;
    highlight_rect_offset_x = -16;
    highlight_rect_offset_y = 4;
  } else if (screen_width <= 1440) {
    speed_min = 14;
    speed_max = speed_min * 2;
    aligned_speed = speed_min * 0.15f;
    font_size = 90;
    line_height = 80;
    match_threshold = 22.0f;
    is_aligned_threshold = 4.0f;
    screen_offset_x = -16.0f;
    char_width = 74.0f;
    highlight_rect_width = char_width;
    highlight_rect_height = 78;
    highlight_rect_offset_x = -16;
    highlight_rect_offset_y = 4;
  } else if (screen_width <= 2048) {
    speed_min = 14;
    speed_max = speed_min * 2;
    aligned_speed = speed_min * 0.15f;
    font_size = 100;
    line_height = 90;
    match_threshold = 26.0f;
    is_aligned_threshold = 4.0f;
    screen_offset_x = -16.0f;
    char_width = 86.0f;
    highlight_rect_width = char_width;
    highlight_rect_height = 84;
    highlight_rect_offset_x = -18;
    highlight_rect_offset_y = 4;
  } else if (screen_width <= 2560) {
    speed_min = 18;
    speed_max = speed_min * 2;
    aligned_speed = speed_min * 0.15f;
    font_size = 150;
    line_height = 140;
    match_threshold = 36.0f;
    is_aligned_threshold = 5.0f;
    screen_offset_x = -30.0f;
    char_width = 118.0f;
    highlight_rect_width = char_width;
    highlight_rect_height = 120;
    highlight_rect_offset_x = -20;
    highlight_rect_offset_y = 12;
  } else {
    speed_min = 24;
    speed_max = speed_min * 2;
    aligned_speed = speed_min * 0.15f;
    font_size = 210;
    line_height = 210.0f;
    match_threshold = 40.0f;
    is_aligned_threshold = 5.0f;
    screen_offset_x = -40.0f;
    char_width = 170.0f;
    highlight_rect_width = 170;
    highlight_rect_height = 180;
    highlight_rect_offset_x = -36;
    highlight_rect_offset_y = 12;
  }
}

#if defined(__cplusplus)
}
#endif
