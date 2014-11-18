#include "PlyModel.h"
#include <sstream>
#include "textfile.h"
#include <random>

using namespace std;

PlyModel* load_ply(string fileName){
	char* fn = new char[fileName.size()+1];
	strcpy(fn, fileName.c_str());
	char* content = textFileRead(fn);

	PlyModel* model = NULL;
	int vertexCount;
	vec4* vertices;

	int faceCount;
	int* indices;

	//Check if file is null
	if(content != NULL){

		stringstream ss(content);
		string line;
		string buffer;
		getline(ss, line); 

		//Check for .PLY extension
		if( line == "ply"){
			getline(ss, buffer);

			ss >> buffer >> buffer >> vertexCount;
			//skip 4 lines
			for(int i = 0; i < 4; i++){
				getline(ss, buffer);
			}
			ss >> buffer >> buffer >> faceCount;
						
			//create arrays
			vertices = new vec4[vertexCount];
			indices = new int[faceCount * 3];

			//skip to end of header
			getline(ss, line);
			cout << line << endl;
			do {
				getline(ss, line);
				//cout << line << endl;
			} while(line != "end_header"); 
			
			//read vertices
			float x, y, z;
			for(int i = 0; i < vertexCount; i++){
				ss >> x;
				ss >> y;
				ss >> z;
				vertices[i] = vec4(x, y, z, 1);
			}

			int buffer;
			//read indices
			for(int i = 0; i < faceCount; i++){
				ss >> buffer; //Hop over the '3'
				ss >> indices[i * 3 + 0];
				ss >> indices[i * 3 + 1];
				ss >> indices[i * 3 + 2];
			}

			vec4* vertexNormals = new vec4[vertexCount];

			vec4* geometry = new vec4[faceCount*3];
			vec4* normals = new vec4[faceCount*3];

			for(int i = 0; i < faceCount; i++){
				geometry[i*3 + 0] = vertices[indices[i*3 + 0]];
				geometry[i*3 + 1] = vertices[indices[i*3 + 1]];
				geometry[i*3 + 2] = vertices[indices[i*3 + 2]];

				vec4 normal = Angel::normalize(Angel::cross((geometry[i * 3 + 1]-geometry[i * 3]), 
															(geometry[i * 3 + 2]-geometry[i * 3])));
				
				vertexNormals[indices[i * 3 + 0]] += normal;
				vertexNormals[indices[i * 3 + 1]] += normal;
				vertexNormals[indices[i * 3 + 2]] += normal;
			}

			for(int i = 0; i < faceCount; i++){
				normals[i * 3 + 0] = Angel::normalize(vertexNormals[indices[i * 3 + 0]]);
				normals[i * 3 + 1] = Angel::normalize(vertexNormals[indices[i * 3 + 1]]);
				normals[i * 3 + 2] = Angel::normalize(vertexNormals[indices[i * 3 + 2]]);
			}


			model = new PlyModel(faceCount*3, geometry, normals);
			free(vertices);
			free(indices);
			free(vertexNormals);
		} else {
			cout << "File is not .Ply Format" << endl;
		}
		
	} else {
		cout << "File is null\n" << endl;
	}
	return model;
}

PlyModel::PlyModel(int vertexCount, vec4* vertices, vec4* normals)
{
	this->vertexCount = vertexCount;
	this->vertices = vertices;
	this->normals = normals;

	this->min = vertices[0];
	this->max = vertices[0];

	for(int v = 0; v < vertexCount; v++){
		this->min.x = (vertices[v].x < this->min.x)?vertices[v].x:this->min.x;
		this->min.y = (vertices[v].y < this->min.y)?vertices[v].y:this->min.y;
		this->min.z = (vertices[v].z < this->min.z)?vertices[v].z:this->min.z;
		
		this->max.x = (vertices[v].x > this->max.x)?vertices[v].x:this->max.x;
		this->max.y = (vertices[v].y > this->max.y)?vertices[v].y:this->max.y;
		this->max.z = (vertices[v].z > this->max.z)?vertices[v].z:this->max.z;
	}	

	this->center = (min+max)/2;
	this->size = (max-min);

	float largestDim = (size.x > size.y)?size.x:size.y;
	largestDim = (largestDim > size.z)?largestDim:size.z;

	this->scaleFactor = 1.0f/largestDim;
	//printf("scale %f\n", scaleFactor);
	//printf("center (%f, %f, %f)\n", this->center.x, this->center.y, this->center.z);
	//printf("size (%f, %f, %f)\n", this->size.x, this->size.y, this->size.z);

}



PlyModel::~PlyModel(void)
{
	free(this->vertices);
	free(this->normals);
}
