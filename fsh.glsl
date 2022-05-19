#version 120

varying vec2 UV;
uniform vec3 backColor;
uniform vec3 foreColor;
uniform sampler2D myTexture;

void main()
{
  float textureAlpha = texture2D(myTexture, UV).r;
  vec3 finalColor = backColor * (1 - textureAlpha) + foreColor * textureAlpha;
  gl_FragColor = vec4(finalColor, 1.0f);
}

