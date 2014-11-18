#pragma once

#include "Angel.h"
#include <string>

using namespace std;

class PlyModel
{

public:
	int vertexCount;
	vec4* vertices;
	vec4* normals;

	GLuint vao; GLuint vbo; GLuint ibo;
	
	vec4 min;
	vec4 max;
	vec4 center;
	vec4 size;

	float scaleFactor;

	PlyModel(int vertexCount, vec4* vertices, vec4* normals);
	~PlyModel(void);

};

PlyModel* load_ply(string fileName);


