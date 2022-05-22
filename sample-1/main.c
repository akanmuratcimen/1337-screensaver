#include "raylib.h"

int main(
  void
) {
  int screenWidth = GetScreenWidth();
  int screenHeight = GetScreenHeight();

  const Color bgColor = { 12, 12, 245, 255 };
  const Color fontColor = { 5, 226, 115, 255 };
  const Color clockBgColor = { 12, 12, 245, 130 };
  const Color clockBorderColor = { 5, 226, 115, 180  };

  //SetConfigFlags(FLAG_WINDOW_RESIZABLE);

  InitWindow(screenWidth, screenHeight, "1337 Screensaver");

  ToggleFullscreen();

  const int bgFontSize = 190;
  const char *bgText = "133713371337133713371337133713371337133713371337133713371337";

  Font bgFont = LoadFontEx("sora.ttf", bgFontSize, 0, 0);
  Image bgTextImage = ImageTextEx(bgFont, bgText, bgFontSize, 0, fontColor);
  Texture2D bgTexture = LoadTextureFromImage(bgTextImage);

  UnloadImage(bgTextImage);

  SetTargetFPS(60);

  const int line_height = 140;

  float scrollingBack = 0;

  while (!WindowShouldClose()) {
    scrollingBack -= 0.3f;

    if (scrollingBack <= -bgTexture.width) {
      scrollingBack = 0;
    }

    BeginDrawing();

      ClearBackground(bgColor);

      for (int i = 0; i < 24; i++) {
        const Vector2 bg1Pos = {
          scrollingBack - 64 * i,
          i * line_height - 24
        };

        DrawTextureEx(
          bgTexture,
          bg1Pos,
          0.0f,
          1.0f,
          WHITE
        );

        const Vector2 bg2Pos = {
          bgTexture.width + scrollingBack - (64 * i),
          i * line_height - 24
        };

        DrawTextureEx(
          bgTexture,
          bg2Pos,
          0.0f,
          1.0f,
          WHITE
        );
      }

    EndDrawing();
  }

  UnloadFont(bgFont);
  UnloadTexture(bgTexture);

  CloseWindow();

  return 0;
}
