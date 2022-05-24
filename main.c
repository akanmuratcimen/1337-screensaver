#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "raylib.h"

#define FULLSCREEN 1
#define FONT_FAMILY "sora.ttf"

#define BG_COLOR CLITERAL(Color){ 12, 12, 245, 255 }
#define FONT_COLOR CLITERAL(Color){ 5, 226, 115, 255 }

#define COLUMN_COUNT 40
#define ROW_COUNT 30

#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)

struct Cell {
  const char *value;
  int column;
  int row;
  Vector2 position;
  float pause_position;
  bool is_highlighting;
  bool is_highlighted;
};

struct Column {
  float speed;
  bool is_highlighting;
  int highlighting_frame_counter;
  float top_position;
};

const char *chars[] = { "1", "3", "7" };

const int font_size = 190;
const float line_height = 140.0f;

const float match_threshold = 40.0f;

const int speed_min = 20;
const int speed_max = 40;

struct Cell cells[COLUMN_COUNT][ROW_COUNT];
struct Column columns[COLUMN_COUNT];

void
initialize_columns(
  void
) {
  for (int i = 0; i < COLUMN_COUNT; i++) {
    columns[i].speed = GetRandomValue(speed_min, speed_max);
    columns[i].top_position = 9999999.9f;
  }
}

void
generate_random_cells(
  void
) {
  for (int i = 0; i < COLUMN_COUNT; i++) {
    for (int j = 0; j < ROW_COUNT; j++) {
      const Vector2 position = {
        i * 110.0f - 40.0f,
        j * line_height - 1000.0f
      };

      struct Cell cell;

      cell.value = chars[GetRandomValue(0, 2)];
      cell.column = i;
      cell.row = j;
      cell.position = position;
      cell.is_highlighting = false;
      cell.is_highlighted = false;
      cell.pause_position = -1.0f;

      cells[i][j] = cell;
    }
  }
}

bool
is_cell_available_to_highlight(
  const struct Cell cell
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
is_cell_match(
  const struct Cell source,
  const struct Cell target,
  const char expected_char
) {
  if (target.value[0] != expected_char) {
    return false;
  }

  if (!is_cell_available_to_highlight(target)) {
    return false;
  }

  if (abs(target.position.y - source.position.y) > match_threshold) {
    return false;
  }

  return true;
}

int main(
  void
) {
  if (!FULLSCREEN) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  }

  InitWindow(GetScreenWidth(), GetScreenHeight(), "1337 Screensaver");

  const Font font = LoadFontEx(FONT_FAMILY, font_size, 0, 0);

  if (FULLSCREEN) {
    ToggleFullscreen();
  }

  SetTargetFPS(30);

  initialize_columns();
  generate_random_cells();

  int refresh_columns_frame_counter = 0;
  int higlighting_activation_frame_counter = 0;

  bool warmup = false;

  while (!WindowShouldClose()) {
    refresh_columns_frame_counter++;
    higlighting_activation_frame_counter++;

    if (!warmup && higlighting_activation_frame_counter > 50) {
      warmup = true;
    }

    if (!warmup) {
      goto draw;
    }

    for (int ci = 0; ci < COLUMN_COUNT - 4; ci++) {
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
          if (is_cell_match(cells[ci][r0i], cells[ci + 1][r1i], '3')) {
            break;
          }
        }

        if (r1i == ROW_COUNT) {
          continue;
        }

        int r2i = 0;
        for (; r2i < ROW_COUNT; r2i++) {
          if (is_cell_match(cells[ci][r0i], cells[ci + 2][r2i], '3')) {
            break;
          }
        }

        if (r2i == ROW_COUNT) {
          continue;
        }

        int r3i = 0;
        for (; r3i < ROW_COUNT; r3i++) {
          if (is_cell_match(cells[ci][r0i], cells[ci + 3][r3i], '7')) {
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

        cells[ci + 0][r0i].is_highlighting = 1;
        cells[ci + 1][r1i].is_highlighting = 1;
        cells[ci + 2][r2i].is_highlighting = 1;
        cells[ci + 3][r3i].is_highlighting = 1;

        columns[ci + 0].is_highlighting = 1;
        columns[ci + 1].is_highlighting = 1;
        columns[ci + 2].is_highlighting = 1;
        columns[ci + 3].is_highlighting = 1;
      }
    }

  draw:
    BeginDrawing();

      ClearBackground(BG_COLOR);

      for (int ci = 0; ci < COLUMN_COUNT; ci++) {
        if (columns[ci].is_highlighting) {
          columns[ci].highlighting_frame_counter++;

          if (columns[ci].highlighting_frame_counter > 60) {
            columns[ci].highlighting_frame_counter = 0;
            columns[ci].is_highlighting = false;

            for (int ri = 0; ri < ROW_COUNT; ri++) {
              if (cells[ci][ri].is_highlighting) {
                cells[ci][ri].is_highlighted = true;
              }

              cells[ci][ri].pause_position = -1;
            }
          }
        }

        for (int ri = 0; ri < ROW_COUNT; ri++) {
          struct Cell cell = cells[ci][ri];

          if (
            columns[ci].is_highlighting &&
            cell.position.y >= cell.pause_position
          ) {
            if (cell.is_highlighting && !cell.is_highlighted) {
              DrawRectangle(
                cell.position.x - 8,
                cell.position.y + 18,
                114,
                146,
                WHITE
              );
            };
          } else {
            cells[ci][ri].position.y += columns[ci].speed / 10;
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

  UnloadFont(font);

  CloseWindow();

  return 0;
}
