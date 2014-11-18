#version 150

uniform mat4 projection_matrix;

in vec4 vPosition;
in vec4 normal;
in vec2 vTexCoord;
out vec2 texCoord;
out vec3 fN;
out vec3 fL;
void main() 
{	
	texCoord = vTexCoord;
	fN = normal.xyz;
	fL = vec3(0.6,1,1);
	gl_Position = projection_matrix*vPosition;
} 
