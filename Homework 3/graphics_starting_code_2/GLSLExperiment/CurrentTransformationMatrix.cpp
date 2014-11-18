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
}

void CurrentTransformationMatrix::pushMatrix(){
	this->modelStack->push_back(plyModel);
}

void CurrentTransformationMatrix::loadIdentity(){
	this->modelStack->clear();
	this->plyModel = Angel::identity();
}

void CurrentTransformationMatrix::Translate(float tx, float ty, float tz){
	plyModel = plyModel * Angel::Translate(tx, ty, tz);
}

void CurrentTransformationMatrix::Translate(vec4 t){
	this->Translate(t.x, t.y, t.z);
}

void CurrentTransformationMatrix::RotateX(float theta){
	plyModel = plyModel * Angel::RotateX(theta);
}

void CurrentTransformationMatrix::RotateY(float theta){
	plyModel = plyModel * Angel::RotateY(theta);
}

void CurrentTransformationMatrix::RotateZ(float theta){
	plyModel = plyModel * Angel::RotateZ(theta);
}

void CurrentTransformationMatrix::Scale(float sx, float sy, float sz){
	plyModel = plyModel * Angel::Scale(sx, sy, sz);
}

void CurrentTransformationMatrix::Scale(float s){
	this->Scale(s, s, s);
}
