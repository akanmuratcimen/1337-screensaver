#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

#define PI 3.14159265358979323846
uniform float uTime = 0.0;
uniform sampler2D texture0;
uniform vec4 colDiffuse;

void main() {
    vec2 fragPos = fragTexCoord;

    fragPos.x -= uTime / 64.0;

    vec4 texelColor = texture2D(texture0, fragPos) * fragColor;

    finalColor = vec4(texelColor.rgb, 1.0);
  }

