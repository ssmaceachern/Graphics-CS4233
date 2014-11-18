#version 150

uniform mat4 projection_matrix;
uniform mat4 model_matrix;
uniform vec4 LightPosition;

in vec4 vPosition;
in vec4 vNormal;

out vec3 tempN; out vec3 tempE; out vec3 tempL;

out vec3 T;
out vec3 R;
void main() 
{
	tempN = (vNormal).xyz;
	tempE = (-1 * (model_matrix*vPosition)).xyz;
	tempL = LightPosition.xyz;

	if(LightPosition.w != 0)
		tempL = LightPosition.xyz - vPosition.xyz;
	

	vec4 eyePos = vPosition;

	vec4 NN = model_matrix * vNormal;
	vec3 N = normalize(NN.xyz);
	
	R = reflect(eyePos.xyz, N.xyz);
	T = refract(eyePos.xyz, N.xyz, 2);
	
	gl_Position = projection_matrix * model_matrix * vPosition;

} 
