#version 120

varying vec3 v;
varying vec3 normal;
varying vec4 ambientGlobal;

void main() {
	ambientGlobal	= gl_LightModel.ambient * gl_FrontMaterial.ambient;
  v = vec3(gl_ModelViewMatrix * gl_Vertex);
  normal = normalize(gl_NormalMatrix * gl_Normal);
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}