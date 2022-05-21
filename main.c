#include "raylib.h"

int main(
  void
) {
  const int screenWidth = GetScreenWidth();
  const int screenHeight = GetScreenHeight();

  const Color bgColor = { 12, 12, 245, 255 };
  const Color fontColor = { 5, 226, 115, 180 };

  InitWindow(screenWidth, screenHeight, "1337 Screensaver");

  ToggleFullscreen();

  const char *bgText = "133713371337133713371337133713371337133713371337133713371337";
  Font font = LoadFontEx("sora.ttf", 96, 0, 0);
  Image bgTextImage = ImageTextEx(font, bgText, 72, 0, fontColor);
  Texture2D bgTexture = LoadTextureFromImage(bgTextImage);
  UnloadImage(bgTextImage);

  SetTargetFPS(60);

  const int line_height = 140;

  int scrollingBack = 0;

  while (!WindowShouldClose()) {
    scrollingBack -= 1.0;

    if (scrollingBack <= -bgTexture.width * 2) {
      scrollingBack = 0;
    }

    BeginDrawing();

      ClearBackground(bgColor);
      
      for (int i = 0; i < 24; i++) {
        const Vector2 bg1Pos = {
          scrollingBack - 64 * i,
          i * line_height - 24
        };

        const Vector2 bg2Pos = {
          bgTexture.width * 2 + scrollingBack - (64 * i),
          i * line_height - 24
        };

        DrawTextureEx(
          bgTexture,
          bg1Pos,
          0.0f,
          2.0f,
          WHITE
        );

        DrawTextureEx(
          bgTexture,
          bg2Pos,
          0.0f,
          2.0f,
          WHITE
        );
      }

    EndDrawing();
  }

  UnloadFont(font);
  UnloadTexture(bgTexture);

  CloseWindow();

  return 0;
}
