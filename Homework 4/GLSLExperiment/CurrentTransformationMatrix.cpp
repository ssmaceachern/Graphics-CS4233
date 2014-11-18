#include "CurrentTransformationMatrix.h"

using namespace std;

CurrentTransformationMatrix::CurrentTransformationMatrix(void){
	this->plyModel = Angel::identity();
	this->modelStack = new vector<mat4>();
}

CurrentTransformationMatrix::~CurrentTransformationMatrix(void){
	//Deconstructor
}

void CurrentTransformationMatrix::popMatrix(){
	this->plyModel = this->modelStack->back();
	this->modelStack->pop_back();
	glUniformMatrix4fv(matrixLocation, 1, TRUE, plyModel);
}

void CurrentTransformationMatrix::pushMatrix(){
	this->modelStack->push_back(plyModel);
	glUniformMatrix4fv(matrixLocation, 1, TRUE, plyModel);
}

void CurrentTransformationMatrix::loadIdentity(){
	this->modelStack->clear();
	this->plyModel = Angel::identity();
	glUniformMatrix4fv(matrixLocation, 1, TRUE, plyModel);
}

void CurrentTransformationMatrix::Translate(float tx, float ty, float tz){
	plyModel = plyModel * Angel::Translate(tx, ty, tz);
	glUniformMatrix4fv(matrixLocation, 1, TRUE, plyModel);
}

void CurrentTransformationMatrix::Translate(vec4 t){
	this->Translate(t.x, t.y, t.z);
	glUniformMatrix4fv(matrixLocation, 1, TRUE, plyModel);
}

void CurrentTransformationMatrix::RotateX(float theta){
	plyModel = plyModel * Angel::RotateX(theta);
	glUniformMatrix4fv(matrixLocation, 1, TRUE, plyModel);
}

void CurrentTransformationMatrix::RotateY(float theta){
	plyModel = plyModel * Angel::RotateY(theta);
	glUniformMatrix4fv(matrixLocation, 1, TRUE, plyModel);
}

void CurrentTransformationMatrix::RotateZ(float theta){
	plyModel = plyModel * Angel::RotateZ(theta);
	glUniformMatrix4fv(matrixLocation, 1, TRUE, plyModel);
}

void CurrentTransformationMatrix::Scale(float sx, float sy, float sz){
	plyModel = plyModel * Angel::Scale(sx, sy, sz);
	glUniformMatrix4fv(matrixLocation, 1, TRUE, plyModel);
}

void CurrentTransformationMatrix::Scale(float s){
	this->Scale(s, s, s);
	glUniformMatrix4fv(matrixLocation, 1, TRUE, plyModel);
}
