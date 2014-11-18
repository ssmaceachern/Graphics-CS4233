#version 150

uniform bool useColor;
in  vec4  interpolatedColor;
out vec4  fColor;

void main() 
{ 
	if(useColor){	
		fColor = interpolatedColor;
	} else {
		fColor = vec4(1.0,1.0,1.0,1.0);
	}
     
} 

