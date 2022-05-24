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
SetRandomColumnSpeeds(
  void
) {
  for (int i = 0; i < COLUMN_COUNT; i++) {
    columns[i].Speed = GetRandomValue(speedMin, speedMax);
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

  SetRandomColumnSpeeds();
  GenerateRandomCells();

  int framesCounter = 0;
  int higlightingActivationFrameCounter = 0;

  while (!WindowShouldClose()) {
    framesCounter++;
    higlightingActivationFrameCounter++;

    if (higlightingActivationFrameCounter > 50) {
      if (higlightingActivationFrameCounter >= INT_MAX) {
        higlightingActivationFrameCounter = 0;
      }

      for (int coli = 0; coli < COLUMN_COUNT - 4; coli++) {
        if (columns[coli].Highlight == 1) {
          continue;
        }

        for (int r0i = 0; r0i < ROW_COUNT; r0i++) {
          if (cells[coli][r0i].Value[0] != '1') {
            continue;
          }

          if (!IsCellAvailableForHighligthing(cells[coli][r0i])) {
            continue;
          }

          int r1i = 0;
          for (; r1i < ROW_COUNT; r1i++) {
            if (IsCellsMatch(cells[coli][r0i], cells[coli + 1][r1i], '3')) {
              break;
            }
          }

          if (r1i == ROW_COUNT) {
            continue;
          }

          int r2i = 0;
          for (; r2i < ROW_COUNT; r2i++) {
            if (IsCellsMatch(cells[coli][r0i], cells[coli + 2][r2i], '3')) {
              break;
            }
          }

          if (r2i == ROW_COUNT) {
            continue;
          }

          int r3i = 0;
          for (; r3i < ROW_COUNT; r3i++) {
            if (IsCellsMatch(cells[coli][r0i], cells[coli + 3][r3i], '7')) {
              break;
            }
          }

          if (r3i == ROW_COUNT) {
            continue;
          }

          float maxY = MAX(
            cells[coli + 0][r0i].Position.y, MAX(
              cells[coli + 1][r1i].Position.y, MAX(
                cells[coli + 2][r2i].Position.y,
                cells[coli + 3][r3i].Position.y
              )
            )
          );

          for (int m = 0; m < ROW_COUNT; m++) {
            cells[coli + 0][m].PausePosition =
              cells[coli + 0][m].Position.y + maxY -
                cells[coli + 0][r0i].Position.y;
          }

          for (int m = 0; m < ROW_COUNT; m++) {
            cells[coli + 1][m].PausePosition =
              cells[coli + 1][m].Position.y + maxY -
                cells[coli + 1][r1i].Position.y;
          }

          for (int m = 0; m < ROW_COUNT; m++) {
            cells[coli + 2][m].PausePosition =
              cells[coli + 2][m].Position.y + maxY -
                cells[coli + 2][r2i].Position.y;
          }

          for (int m = 0; m < ROW_COUNT; m++) {
            cells[coli + 3][m].PausePosition =
              cells[coli + 3][m].Position.y + maxY -
                cells[coli + 3][r3i].Position.y;
          }

          cells[coli + 0][r0i].Highlight = 1;
          cells[coli + 1][r1i].Highlight = 1;
          cells[coli + 2][r2i].Highlight = 1;
          cells[coli + 3][r3i].Highlight = 1;

          columns[coli + 0].Highlight = 1;
          columns[coli + 1].Highlight = 1;
          columns[coli + 2].Highlight = 1;
          columns[coli + 3].Highlight = 1;
        }
      }
    }

    BeginDrawing();

      ClearBackground(BG_COLOR);

      for (int i = 0; i < COLUMN_COUNT; i++) {
        if (columns[i].Highlight == 1) {
          columns[i].HighlightFrameCounter++;

          if (columns[i].HighlightFrameCounter > 60) {
            columns[i].HighlightFrameCounter = 0;
            columns[i].Highlight = 0;

            for (int m = 0; m < ROW_COUNT; m++) {
              if (cells[i][m].Highlight == 1) {
                cells[i][m].IsHighlighted = 1;
              }

              cells[i][m].PausePosition = -1;
            }
          }
        }

        for (int j = 0; j < ROW_COUNT; j++) {
          struct Cell cell = cells[i][j];

          if (
            columns[i].Highlight == 1 &&
            cell.Position.y >= cell.PausePosition
          ) {
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
            cells[i][j].Position.y += columns[i].Speed / 10;
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

    if (((framesCounter / 100) % 2) == 1) {
      framesCounter = 0;

      SetRandomColumnSpeeds();

      for (int j = 0; j < COLUMN_COUNT; j++) {
        columns[j].TopPosition = FLT_MAX;
      }

      for (int i = 0; i < COLUMN_COUNT; i++) {
        if (columns[i].Highlight == 1) {
          continue;
        }

        for (int j = 0; j < ROW_COUNT; j++) {
          columns[i].TopPosition = MIN(columns[i].TopPosition, cells[i][j].Position.y);
        }

        for (int j = 0; j < ROW_COUNT; j++) {
          if (cells[i][j].Position.y > GetScreenHeight()) {
            columns[i].TopPosition = columns[i].TopPosition - lineHeight;

            cells[i][j].IsHighlighted = 0;
            cells[i][j].Highlight = 0;
            cells[i][j].Position.y = columns[i].TopPosition;
            cells[i][j].Value = chars[GetRandomValue(0, 2)];
          }
        }
      }
    }
  }

  UnloadFont(font);

  CloseWindow();

  return 0;
}
