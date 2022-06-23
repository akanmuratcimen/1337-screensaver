#version 330 core

in vec2 out_texture_coord;
uniform sampler2D tex;

void main() {
  gl_FragColor = texture(tex, out_texture_coord);
}

