#version 120

attribute vec3 vertexPosition;
attribute vec2 vertexUV;
varying vec2 UV;

void main()
{
  gl_Position = vec4(vertexPosition, 1.0);
  UV = vertexUV;
}

