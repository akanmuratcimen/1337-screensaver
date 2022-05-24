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
  const char *Value;
  int Column;
  int Row;
  Vector2 Position;
  float PausePosition;
  int Highlight;
  int IsHighlighted;
};

struct Column {
  float Speed;
  int Highlight;
  int HighlightFrameCounter;
  float TopPosition;
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
    columns[i].Speed = GetRandomValue(speedMin, speedMax);
    columns[i].TopPosition = FLT_MAX;
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

      cell.Value = chars[GetRandomValue(0, 2)];
      cell.Column = i;
      cell.Row = j;
      cell.Position = position;
      cell.Highlight = 0;
      cell.IsHighlighted = 0;
      cell.PausePosition = -1.0f;

      cells[i][j] = cell;
    }
  }
}

bool
IsCellAvailableForHighligthing(
  const struct Cell cell
) {
  if (columns[cell.Column].Highlight == 1) {
    return false;
  }

  if (cell.IsHighlighted == 1) {
    return false;
  }

  if (cell.Position.y < 0) {
    return false;
  }

  if (cell.Position.y > GetScreenHeight()) {
    return false;
  }

  if (cell.Position.x > GetScreenWidth()) {
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
  if (target.Value[0] != expectedChar) {
    return false;
  }

  if (!IsCellAvailableForHighligthing(target)) {
    return false;
  }

  if (abs(target.Position.y - source.Position.y) > matchThreshold) {
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
        if (columns[ci].Highlight == 1) {
          continue;
        }

        for (int r0i = 0; r0i < ROW_COUNT; r0i++) {
          if (cells[ci][r0i].Value[0] != '1') {
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
            cells[ci + 0][r0i].Position.y, MAX(
              cells[ci + 1][r1i].Position.y, MAX(
                cells[ci + 2][r2i].Position.y,
                cells[ci + 3][r3i].Position.y
              )
            )
          );

          for (int ri = 0; ri < ROW_COUNT; ri++) {
            cells[ci + 0][ri].PausePosition =
              cells[ci + 0][ri].Position.y + maxY -
                cells[ci + 0][r0i].Position.y;
          }

          for (int ri = 0; ri < ROW_COUNT; ri++) {
            cells[ci + 1][ri].PausePosition =
              cells[ci + 1][ri].Position.y + maxY -
                cells[ci + 1][r1i].Position.y;
          }

          for (int ri = 0; ri < ROW_COUNT; ri++) {
            cells[ci + 2][ri].PausePosition =
              cells[ci + 2][ri].Position.y + maxY -
                cells[ci + 2][r2i].Position.y;
          }

          for (int ri = 0; ri < ROW_COUNT; ri++) {
            cells[ci + 3][ri].PausePosition =
              cells[ci + 3][ri].Position.y + maxY -
                cells[ci + 3][r3i].Position.y;
          }

          cells[ci + 0][r0i].Highlight = 1;
          cells[ci + 1][r1i].Highlight = 1;
          cells[ci + 2][r2i].Highlight = 1;
          cells[ci + 3][r3i].Highlight = 1;

          columns[ci + 0].Highlight = 1;
          columns[ci + 1].Highlight = 1;
          columns[ci + 2].Highlight = 1;
          columns[ci + 3].Highlight = 1;
        }
      }
    }

    BeginDrawing();

      ClearBackground(BG_COLOR);

      for (int ci = 0; ci < COLUMN_COUNT; ci++) {
        if (columns[ci].Highlight == 1) {
          columns[ci].HighlightFrameCounter++;

          if (columns[ci].HighlightFrameCounter > 60) {
            columns[ci].HighlightFrameCounter = 0;
            columns[ci].Highlight = 0;

            for (int ri = 0; ri < ROW_COUNT; ri++) {
              if (cells[ci][ri].Highlight == 1) {
                cells[ci][ri].IsHighlighted = 1;
              }

              cells[ci][ri].PausePosition = -1;
            }
          }
        }

        for (int ri = 0; ri < ROW_COUNT; ri++) {
          struct Cell cell = cells[ci][ri];

          if (columns[ci].Highlight == 1 && cell.Position.y >= cell.PausePosition) {
            if (cell.Highlight == 1 && cell.IsHighlighted == 0) {
              DrawRectangle(
                cell.Position.x - 8,
                cell.Position.y + 18,
                114,
                146,
                WHITE
              );
            };
          } else {
            cells[ci][ri].Position.y += columns[ci].Speed / 10;
          }

          DrawTextEx(
            font,
            cell.Value,
            cell.Position,
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
        if (columns[ci].Highlight == 1) {
          continue;
        }

        for (int ri = 0; ri < ROW_COUNT; ri++) {
          columns[ci].TopPosition = MIN(
            columns[ci].TopPosition,
            cells[ci][ri].Position.y
          );
        }

        for (int ri = 0; ri < ROW_COUNT; ri++) {
          if (cells[ci][ri].Position.y > GetScreenHeight()) {
            columns[ci].TopPosition = columns[ci].TopPosition - lineHeight;

            cells[ci][ri].IsHighlighted = 0;
            cells[ci][ri].Highlight = 0;
            cells[ci][ri].Position.y = columns[ci].TopPosition;
            cells[ci][ri].Value = chars[GetRandomValue(0, 2)];
          }
        }
      }
    }
  }

  UnloadFont(font);

  CloseWindow();

  return 0;
}
