#version 460 core
in vec2 TexCoords;
out vec4 color;

layout (binding = 0) uniform sampler2D text;
layout (location = 7) uniform vec3 textColor;
layout (location = 2) uniform vec3 bgColor;

void main() {
  float textureAlpha = texture(text, TexCoords).r;
  vec3 finalColor = bgColor * (1 - textureAlpha) + textColor * textureAlpha;
  color = vec4(finalColor, 1.0f);
}

