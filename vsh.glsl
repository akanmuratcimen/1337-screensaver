#version 120

attribute vec3 vertexPosition;
attribute vec2 vertexUV;
varying vec2 UV;

void main()
{
  gl_Position = vec4(vertexPosition.xy, 0.0, 1.0);
  UV = vertexUV;
}

