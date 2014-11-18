
#include "Angel.h"
#include "PlyModel.h"
#include <sstream>
#include "CurrentTransformationMatrix.h"
#include <random>
#include <iostream>;
#include <time.h>
#include "textfile.h"
#include "bmpread.h"
//----------------------------------------------------------------------------

using namespace std;

int width = 0;
int height = 0;

void init(void);
void initRegShaders(void);
void initLighting(void);

void loadModels(void);
void loadCowModel(void);
void loadGround(void);
void loadWalls(void);

void display( void );
void keyboard( unsigned char key, int x, int y );
void resize(int w, int h);
void idle();

void buffer_ply(PlyModel* model);
void Recolor(float r, float g, float b, float a);

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

// handle to program
GLuint vProgram;
GLuint viewMatrix;
GLuint vPosition; GLuint vNormal; GLuint vColor;

GLuint lightPosition;
GLuint ambient, diffuse, specular;
GLuint shine;
GLuint shadow;
GLuint vCube;

GLuint wProgram;
GLuint wPosition; GLuint wNormal; GLuint wColor;
GLuint wProj; GLuint wModel;
GLuint wUseTex;
GLuint wTex;
GLuint wTexture;

CurrentTransformationMatrix modelView;
mat4 proj;

vec4 lightPos(1.0, 1.0, 1.0, 1.0);
mat4 lightProjX = Angel::identity();
mat4 lightProjY = Angel::identity();
mat4 lightProjZ = Angel::identity();

PlyModel* cow;

GLuint ground;
GLuint walls;
GLuint cubeMap;
GLuint cubeTex;
GLuint grassTexture;
GLuint stoneTexture;

GLuint vColorType;

bool shadowsOn = true;
bool texturesOn = true;

int ct = 1;
int lastTime;

float rotation = 0.0f;

//Helper function to change the ambient and diffuse coloring
void Recolor(float r, float g, float b, float a){	
	glUniform4fv(ambient, 1, vec4(r, g, b, a)*0.5f);
	glUniform4fv(diffuse, 1, vec4(r, g, b, a)*0.5f);
}

void initRegShaders(){
	//init shaders
	vProgram = InitShader("vshader1.glsl", "fshader1.glsl");
	modelView.matrixLocation = glGetUniformLocationARB(vProgram, "model_matrix");
	viewMatrix = glGetUniformLocationARB(vProgram, "projection_matrix");
	vPosition = glGetAttribLocation(vProgram, "vPosition");
	vNormal = glGetAttribLocation(vProgram, "vNormal");
	vColor = glGetUniformLocation(vProgram, "color");
}

void initLightShaders(){
	lightPosition = glGetUniformLocation(vProgram, "LightPosition");
	ambient = glGetUniformLocation(vProgram, "ambient");
	diffuse = glGetUniformLocation(vProgram, "diffuse");
	specular = glGetUniformLocation(vProgram, "specular");
	shine = glGetUniformLocation(vProgram, "shine");
	shadow = glGetUniformLocation(vProgram, "shadow");
}

void initCubeMapShaders(){
	vCube = glGetUniformLocation(vProgram, "cube");
	vColorType = glGetUniformLocation(vProgram, "ct");
}

void initWallShaders(){
	wProgram = InitShader("vshader_w.glsl", "fshader_w.glsl");
	wProj = glGetUniformLocationARB(wProgram, "projection_matrix");
	wUseTex = glGetUniformLocation(wProgram, "useTex");
	wColor = glGetUniformLocation(wProgram, "color");
	wPosition = glGetAttribLocation(wProgram, "vPosition");
	wNormal = glGetAttribLocation(wProgram, "normal");
	wTex = glGetAttribLocation(wProgram, "vTexCoord");
	wTexture = glGetUniformLocation(wProgram, "texture");
}

void init(void)
{	
	//Init shaders
	initRegShaders();
	initLightShaders();
	initCubeMapShaders();
	initWallShaders();

	glUseProgram(vProgram);
	modelView.loadIdentity();

	initLighting();	
	
	Recolor(1, 0, 0, 1);
	// sets the default color to clear screen
    
	glClearColor( 0.0, 0.0, 0.0, 1.0 ); // black background
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

}

void initLighting(){
	glUniform4fv(lightPosition, 1, lightPos);
	glUniform4fv(ambient, 1, vec4(0.2f, 0.2f, 0.2f, 1));
	glUniform4fv(diffuse, 1, vec4(0.6f, 0.2f, 0.2f, 1));
	glUniform4fv(specular, 1, vec4(0.9f, 0.9f, 0.9f, 1));
	glUniform1f(shine, 60);
	glUniform1i(vColorType, 1);

	lightProjX[3][1] = -1.0		/	lightPos.x;
	lightProjY[3][1] = -1.0		/	lightPos.y;
	lightProjZ[3][1] = -1.0		/	lightPos.z;
}

string cubeFileLocations[6] = {
	"bmp_files//nvposx.bmp",
	"bmp_files//nvnegx.bmp",
	"bmp_files//nvposy.bmp",
	"bmp_files//nvnegy.bmp",
	"bmp_files//nvposz.bmp",
	"bmp_files//nvnegz.bmp"
};

//Read in .BMP image files and generate a cube map
void loadCubeMap(){
	glEnable(GL_TEXTURE_CUBE_MAP);
	glGenTextures(1, &cubeTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

	bmpread_t cubeBMP[6];
	for(int i = 0; i < 6; i++){
		if(!bmpread(cubeFileLocations[i].c_str(), 0, &cubeBMP[i])){
			printf("Couldn't open %s\n", cubeFileLocations[i].c_str());
			exit(1);
		}
		printf("Loading %s\n", cubeFileLocations[i].c_str());
	}

	glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, cubeBMP[0].width, cubeBMP[0].height, 0, GL_RGB, GL_UNSIGNED_BYTE, cubeBMP[0].rgb_data );
	glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, cubeBMP[1].width, cubeBMP[1].height, 0, GL_RGB, GL_UNSIGNED_BYTE, cubeBMP[1].rgb_data );
	glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, cubeBMP[2].width, cubeBMP[2].height, 0, GL_RGB, GL_UNSIGNED_BYTE, cubeBMP[2].rgb_data );
	glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, cubeBMP[3].width, cubeBMP[3].height, 0, GL_RGB, GL_UNSIGNED_BYTE, cubeBMP[3].rgb_data );
	glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, cubeBMP[4].width, cubeBMP[4].height, 0, GL_RGB, GL_UNSIGNED_BYTE, cubeBMP[4].rgb_data );
	glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, cubeBMP[5].width, cubeBMP[5].height, 0, GL_RGB, GL_UNSIGNED_BYTE, cubeBMP[5].rgb_data );

	for(int i = 0; i < 6; i++){
		bmpread_free(&cubeBMP[i]);
	}

	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glUseProgram(vProgram);
	glUniform1i(vCube, cubeTex);
}

//Load .PLY data into buffer
void buffer_ply(PlyModel* model){
	
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);	

	GLuint buffer;
	glGenBuffers(1, &buffer);

	model->vao = vao;
	model->vbo = buffer;
	
	glUseProgram(vProgram);
	int vSize = model->vertexCount*sizeof(vec4);

	glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
	glBufferData(GL_ARRAY_BUFFER, vSize*2, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, model->vertices);
	glBufferSubData(GL_ARRAY_BUFFER, vSize, vSize, model->normals);
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vSize));

}


void loadCowModel(){
	cow = load_ply("ply_files/cow.ply");
	buffer_ply(cow);
}

float wallVerts[] = {
	//Vertices
	-1.0f, -1.0f, -1.0f, 1,
	 1.0f, -1.0f, -1.0f, 1,
	 1.0f,  1.0f, -1.0f, 1,
	-1.0f,  1.0f, -1.0f, 1,
	
	-1.0f, -1.0f,  1.0f, 1,
	-1.0f, -1.0f, -1.0f, 1,
	-1.0f,  1.0f, -1.0f, 1,
	-1.0f,  1.0f,  1.0f, 1,
	
	//Normals
	0,0,1,1,
	0,0,1,1,
	0,0,1,1,
	0,0,1,1,
	1,0,0,1,
	1,0,0,1,
	1,0,0,1,
	1,0,0,1,
				
	0,0,
	4,0,
	4,4,
	0,4,
	0,0,
	4,0,
	4,4,
	0,4,
};

void loadWalls(){

	bmpread_t stoneBMP;
	if(!bmpread("bmp_files/stones.bmp", 0, &stoneBMP)){
		cout << "Couldn't load stone tex!\n";
		exit(1);
	}

	glActiveTexture(GL_TEXTURE2);
	glGenTextures(1, &stoneTexture);
	glBindTexture(GL_TEXTURE_2D, stoneTexture);

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	 glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, stoneBMP.width, stoneBMP.height, 0, GL_RGB, GL_UNSIGNED_BYTE, stoneBMP.rgb_data );
	bmpread_free(&stoneBMP);

	glGenVertexArrays(1, &walls);
	glBindVertexArray(walls);
	GLuint wallBuf;
	glGenBuffers(1, &wallBuf);
	glBindBuffer(GL_ARRAY_BUFFER, wallBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallVerts), wallVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(wPosition);
	glVertexAttribPointer(wPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(wNormal);
	glVertexAttribPointer(wNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(128));
	glEnableVertexAttribArray(wTex);
	glVertexAttribPointer(wTex, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(256));
}

float groundVerts[] = {
	//Vertices
	-1.0, -1.0f,  1.0f, 1,
	-1.0, -1.0f, -1.0f, 1,
	 1.0, -1.0f, -1.0f, 1,
	 1.0, -1.0f,  1.0f, 1,

	 //Normals
	 0,1,0,0,
	 0,1,0,0,
	 0,1,0,0,
	 0,1,0,0,

	 0,4,
	 0,0,
	 4,0,
	 4,4,

	};

void loadGround(){
	bmpread_t grassBMP;
	if(!bmpread("bmp_files/grass.bmp", 0, &grassBMP)){
		cout << "Couldn't load grass tex!\n";
		exit(1);
	}

	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &grassTexture);
	glBindTexture(GL_TEXTURE_2D, grassTexture);

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	 glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, grassBMP.width, grassBMP.height, 0, GL_RGB, GL_UNSIGNED_BYTE, grassBMP.rgb_data );
	bmpread_free(&grassBMP);

	glUseProgram(wProgram);
	glGenVertexArrays(1, &ground);
	glBindVertexArray(ground);
	GLuint groundBuf;
	glGenBuffers(1, &groundBuf);
	glBindBuffer(GL_ARRAY_BUFFER, groundBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(groundVerts), groundVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(wPosition);
	glVertexAttribPointer(wPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(wNormal);
	glVertexAttribPointer(wNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(64));
	glEnableVertexAttribArray(wTex);
	glVertexAttribPointer(wTex, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(128));
}

//Load all of the models for the scene
void loadModels(){
	loadCubeMap();
	loadCowModel();
	loadGround();
	loadWalls();
}


//----------------------------------------------------------------------------
// this is where the drawing should happen
void display( void )
{
		
	glEnable( GL_DEPTH_TEST );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     // clear the window
	
	glUseProgram(wProgram);
	
	//enable textures
	glUniform1i(wUseTex, texturesOn);
	glUniform4fv(wColor, 1,  vec4(0.9, 0.9, 0.9, 1));
	glUniformMatrix4fv(wProj, 1, GL_TRUE, proj);
	
	//draw ground
	glUniform1i(wTexture, 1);
	glBindVertexArray(ground);
	glDrawArrays(GL_QUADS, 0, 4);
	
	//draw walls
	glUniform1i(wTexture, 2);
	glUniform4fv(wColor, 1,  vec4(0.9, 0.9, 0.9, 1));
	glBindVertexArray(walls);
	glDrawArrays(GL_QUADS, 0, 8);
	
	glUseProgram(vProgram);
	glUniform1i(vCube, 0);
	glUniform1i(vColorType, ct);
	glUniformMatrix4fv( viewMatrix, 1, GL_TRUE, proj);
	glBindVertexArray(cow->vao);
	
	Recolor(1,1,1,1);

	modelView.pushMatrix();
	
		modelView.Translate(-cow->center + vec3(0.0f, 0.5f, 0.0f));
		modelView.Scale(cow->scaleFactor*1.5f);
		modelView.RotateY(rotation);
		glUniform4fv(lightPosition, 1, vec4(cos(Angel::DegreesToRadians * rotation), 1.0, sin(DegreesToRadians * rotation), 0));
		glDrawArrays(GL_TRIANGLES, 0, cow->vertexCount);

		if(shadowsOn){
			Recolor(0,0,0,1);
			
			glUniform1i(shadow, true);
			modelView.pushMatrix();

				modelView.Scale(1, 0, 1);
				modelView.Translate(0, -0.9, 0);
				
				glDrawArrays(GL_TRIANGLES, 0, cow->vertexCount);

			modelView.popMatrix();

			glUniform1i(shadow, false);
		}

	modelView.popMatrix();


    glFlush(); // force output to graphics hardware

	// use this call to double buffer
	glutSwapBuffers();
	
	glDisable( GL_DEPTH_TEST ); 
	
}

//----------------------------------------------------------------------------

// keyboard handler
void keyboard( unsigned char key, int x, int y )
{
	bool newPly = false;

    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;
	
	case 'a':
		shadowsOn = !shadowsOn;
		break;
	
	case 'b':
		texturesOn = !texturesOn;
		break;
	
	case 'c':
		if(ct == 2){
			ct = 1;
		}
		ct = 2;
		break;
	
	case 'd':
		if(ct == 3){
			ct = 1;
		}
		ct = 3;
		break;
    }

	display();
}

//resize handler
void resize(int w, int h){
	width = w;
	height = h;

	glViewport(0, 0, width, height);

	proj = Angel::Perspective((GLfloat)55.0, (GLfloat)width/(GLfloat)height, (GLfloat)0.1, (GLfloat)110.0);
	proj = proj * Angel::LookAt(vec4(1.0f, 0.5f, 2.5f, 1.0f), vec4(0.0f,0.0f,0.0f,1.0f), vec4(0.0f,1.0f,0.0f,1.0f));
}

void idle(){
	int time = glutGet(GLUT_ELAPSED_TIME);
	float delta = (float)(time-lastTime)/1000.0f;
	lastTime = time;

	rotation += 45.0f * delta;
	
	glutPostRedisplay();
	
}

//----------------------------------------------------------------------------
// entry point
int main( int argc, char **argv )
{
	srand(time(NULL));

	// init glut
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 1024, 768 );
	width = 1024;
	height = 768;

	// create window
	// opengl can be incorperated into other packages like wxwidgets, fltoolkit, etc.
    glutCreateWindow( "Sean MacEachern - Homework 4" );

	// init glew
    glewInit();
	init();
    loadModels();

	// assign handlers
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
	glutReshapeFunc(resize);
	glutIdleFunc(idle);

	// enter the drawing loop
	// frame rate can be controlled with 
	lastTime = glutGet(GLUT_ELAPSED_TIME);
	glutMainLoop();

    return 0;
}
