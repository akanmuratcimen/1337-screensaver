#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 offset;
layout (location = 2) in vec2 tex_coord;
layout (location = 3) in vec2 tex_offset;

uniform mat4 transform;

out vec2 out_texture_coord;

void main() {
  gl_Position = transform * vec4(position + offset, 0.0f, 1.0f);
  out_texture_coord = tex_coord + tex_offset;
}

