#pragma once
#include "Angel.h"
#include <vector>

using namespace std;

class CurrentTransformationMatrix
{
public:
	CurrentTransformationMatrix(void);
	~CurrentTransformationMatrix(void);

	mat4 plyModel;
	vector<mat4>* modelStack;
	GLuint matrixLocation;

	void pushMatrix();
	void popMatrix();
	void loadIdentity();

	void Translate(float tx, float ty, float tz);
	void Translate(vec4 t);
	
	void RotateX(float theta);
	void RotateY(float theta);
	void RotateZ(float theta);

	void Scale(float sx, float sy, float sz);
	void Scale(float s);

	//This function is only applied in the x-axis
	void Shear(float x);
};

