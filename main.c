#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>
#include <float.h>

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
  float pausePosition;
  int highlight;
  int isHighlighted;
};

struct Column {
  float speed;
  int highlight;
  int highlightFrameCounter;
  float topPosition;
};

const char *chars[] = { "1", "3", "7" };

const int fontSize = 190;
const float lineHeight = 140.0f;

const float matchThreshold = 40.0f;

const int speedMin = 20;
const int speedMax = 40;

struct Cell cells[COLUMN_COUNT][ROW_COUNT];
struct Column columns[COLUMN_COUNT];

void
InitializeColumns(
  void
) {
  for (int i = 0; i < COLUMN_COUNT; i++) {
    columns[i].speed = GetRandomValue(speedMin, speedMax);
    columns[i].topPosition = FLT_MAX;
  }
}

void
GenerateRandomCells(
  void
) {
  for (int i = 0; i < COLUMN_COUNT; i++) {
    for (int j = 0; j < ROW_COUNT; j++) {
      const Vector2 position = {
        i * 110.0f - 40.0f,
        j * lineHeight - 1000.0f
      };

      struct Cell cell;

      cell.value = chars[GetRandomValue(0, 2)];
      cell.column = i;
      cell.row = j;
      cell.position = position;
      cell.highlight = 0;
      cell.isHighlighted = 0;
      cell.pausePosition = -1.0f;

      cells[i][j] = cell;
    }
  }
}

bool
IsCellAvailableForHighligthing(
  const struct Cell cell
) {
  if (columns[cell.column].highlight == 1) {
    return false;
  }

  if (cell.isHighlighted == 1) {
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
IsCellsMatch(
  const struct Cell source,
  const struct Cell target,
  const char expectedChar
) {
  if (target.value[0] != expectedChar) {
    return false;
  }

  if (!IsCellAvailableForHighligthing(target)) {
    return false;
  }

  if (abs(target.position.y - source.position.y) > matchThreshold) {
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

  const Font font = LoadFontEx(FONT_FAMILY, fontSize, 0, 0);

  if (FULLSCREEN) {
    ToggleFullscreen();
  }

  SetTargetFPS(30);

  InitializeColumns();
  GenerateRandomCells();

  int refreshColumnsFrameCounter = 0;
  int higlightingActivationFrameCounter = 0;

  while (!WindowShouldClose()) {
    refreshColumnsFrameCounter++;
    higlightingActivationFrameCounter++;

    if (higlightingActivationFrameCounter > 50) {
      if (higlightingActivationFrameCounter >= INT_MAX) {
        higlightingActivationFrameCounter = 0;
      }

      for (int ci = 0; ci < COLUMN_COUNT - 4; ci++) {
        if (columns[ci].highlight == 1) {
          continue;
        }

        for (int r0i = 0; r0i < ROW_COUNT; r0i++) {
          if (cells[ci][r0i].value[0] != '1') {
            continue;
          }

          if (!IsCellAvailableForHighligthing(cells[ci][r0i])) {
            continue;
          }

          int r1i = 0;
          for (; r1i < ROW_COUNT; r1i++) {
            if (IsCellsMatch(cells[ci][r0i], cells[ci + 1][r1i], '3')) {
              break;
            }
          }

          if (r1i == ROW_COUNT) {
            continue;
          }

          int r2i = 0;
          for (; r2i < ROW_COUNT; r2i++) {
            if (IsCellsMatch(cells[ci][r0i], cells[ci + 2][r2i], '3')) {
              break;
            }
          }

          if (r2i == ROW_COUNT) {
            continue;
          }

          int r3i = 0;
          for (; r3i < ROW_COUNT; r3i++) {
            if (IsCellsMatch(cells[ci][r0i], cells[ci + 3][r3i], '7')) {
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
            cells[ci + 0][ri].pausePosition =
              cells[ci + 0][ri].position.y + maxY -
                cells[ci + 0][r0i].position.y;
          }

          for (int ri = 0; ri < ROW_COUNT; ri++) {
            cells[ci + 1][ri].pausePosition =
              cells[ci + 1][ri].position.y + maxY -
                cells[ci + 1][r1i].position.y;
          }

          for (int ri = 0; ri < ROW_COUNT; ri++) {
            cells[ci + 2][ri].pausePosition =
              cells[ci + 2][ri].position.y + maxY -
                cells[ci + 2][r2i].position.y;
          }

          for (int ri = 0; ri < ROW_COUNT; ri++) {
            cells[ci + 3][ri].pausePosition =
              cells[ci + 3][ri].position.y + maxY -
                cells[ci + 3][r3i].position.y;
          }

          cells[ci + 0][r0i].highlight = 1;
          cells[ci + 1][r1i].highlight = 1;
          cells[ci + 2][r2i].highlight = 1;
          cells[ci + 3][r3i].highlight = 1;

          columns[ci + 0].highlight = 1;
          columns[ci + 1].highlight = 1;
          columns[ci + 2].highlight = 1;
          columns[ci + 3].highlight = 1;
        }
      }
    }

    BeginDrawing();

      ClearBackground(BG_COLOR);

      for (int ci = 0; ci < COLUMN_COUNT; ci++) {
        if (columns[ci].highlight == 1) {
          columns[ci].highlightFrameCounter++;

          if (columns[ci].highlightFrameCounter > 60) {
            columns[ci].highlightFrameCounter = 0;
            columns[ci].highlight = 0;

            for (int ri = 0; ri < ROW_COUNT; ri++) {
              if (cells[ci][ri].highlight == 1) {
                cells[ci][ri].isHighlighted = 1;
              }

              cells[ci][ri].pausePosition = -1;
            }
          }
        }

        for (int ri = 0; ri < ROW_COUNT; ri++) {
          struct Cell cell = cells[ci][ri];

          if (columns[ci].highlight == 1 && cell.position.y >= cell.pausePosition) {
            if (cell.highlight == 1 && cell.isHighlighted == 0) {
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
            fontSize,
            0,
            FONT_COLOR
          );
        }
      }

    EndDrawing();

    if (((refreshColumnsFrameCounter / 100) % 2) == 1) {
      refreshColumnsFrameCounter = 0;

      InitializeColumns();

      for (int ci = 0; ci < COLUMN_COUNT; ci++) {
        if (columns[ci].highlight == 1) {
          continue;
        }

        for (int ri = 0; ri < ROW_COUNT; ri++) {
          columns[ci].topPosition = MIN(
            columns[ci].topPosition,
            cells[ci][ri].position.y
          );
        }

        for (int ri = 0; ri < ROW_COUNT; ri++) {
          if (cells[ci][ri].position.y > GetScreenHeight()) {
            columns[ci].topPosition = columns[ci].topPosition - lineHeight;

            cells[ci][ri].isHighlighted = 0;
            cells[ci][ri].highlight = 0;
            cells[ci][ri].position.y = columns[ci].topPosition;
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
