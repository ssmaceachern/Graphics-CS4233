#version 150

uniform mat4 projection_matrix;
uniform mat4 model_matrix;
uniform float shear;
uniform float twist;
in  vec4 vPosition;
in  vec4 vColor;

out vec4 interpolatedColor;

void main() 
{
  mat4 shearX = mat4( 
		1.0, 0.0, 0.0, 0.0,
		shear, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
  );

  vec4 norm = model_matrix*vPosition;
  float c = cos(norm.y*twist);
  float s = sin(norm.y*twist);

  mat4 twist = mat4( 
		c, 0.0, s, 0.0,
		0.0, 1.0, 0.0, 0.0,
		-s, 0.0, c, 0.0,
		0.0, 0.0, 0.0, 1.0
  ); 

  gl_Position = projection_matrix*shearX*twist*model_matrix*vPosition;
  interpolatedColor = vColor;
} 
