#version 150

uniform vec4 color;
uniform bool useTex;
uniform sampler2D texture;
in vec2 texCoord;
in vec3 fN;
in vec3 fL;
out vec4 fColor;

void main() 
{ 

	vec4 diffuse;
	vec4 ambient;
	//specular is always 0 for walls

	if(useTex){
		diffuse =  texture2D( texture, texCoord);
	} else {
		diffuse = color;
	}
	
	ambient = diffuse*0.2;

	vec3 N = normalize(fN);
	vec3 L = normalize(fL);
	float kd = max(dot(L,N),0.0);
	fColor = kd*diffuse + ambient;
	fColor.a = 1.0;	


     
} 

