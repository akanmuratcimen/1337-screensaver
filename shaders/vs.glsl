#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aOffset;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec2 aTexOffset;

out vec2 TexCoord;

uniform mat4 transform;

void main() {
  gl_Position = transform * vec4(aPos + aOffset, 0.0f, 1.0f);
  TexCoord = aTexCoord + aTexOffset;
}

