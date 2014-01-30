#version 120

uniform sampler2D texture;
uniform float size = 1/1024.f;

const float offset[3] = float[] (0.0, 1.3846153846, 3.2307692308);
const float weight[3] = float[] (0.2270270270, 0.3162162162, 0.0702702703);

float luminance (vec4 color) {
  return dot(vec3(.17f, .51f, .32f), color.rgb);
}

void main () {
  vec2 coord = gl_TexCoord[0].xy;
  
  vec4 color = texture2D(texture, coord) * weight[0];
  float s = 4.f * size;
  color += texture2D(texture, coord + vec2(0, offset[1])*s) * weight[1];
  color += texture2D(texture, coord - vec2(0, offset[1])*s) * weight[1];
  color += texture2D(texture, coord + vec2(0, offset[2])*s) * weight[2];
  color += texture2D(texture, coord - vec2(0, offset[2])*s) * weight[2];
  
  gl_FragColor = color;
}