

#include "Angel.h"
#include "PlyModel.h"
#include "CurrentTransformationMatrix.h"
#include "textfile.h"
#include "Rule.h"

#include <random>
#include <iostream>
#include <time.h>
#include <sstream>
#include <map>

using namespace std;

//*------------------------------------------------------------------------*//

int width = 0;
int height = 0;
const float speed = 0.05f;

void init(void);
void display( void );
void keyboard( unsigned char key, int x, int y );
void resize(int w, int h);
void idle();

void load_ply(void);
void buffer_ply(PlyModel* model);
void draw_ply(PlyModel* model);

void draw_lsys(Rule* rule);
string rewrite_pattern(string str);
void rewrite_lsysStr();

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

map <char, Rule*> trees;

// handle to program
GLuint program;
GLuint modelMatrix;
GLuint viewMatrix;
GLuint vPosition;
GLuint vColor;
GLuint vUseColor;
GLuint vShear;
GLuint vTwist;

CurrentTransformationMatrix modelView;
mat4 proj;
bool useColor = false;
float shear = 0.0f;
float twist = 0.0f;


PlyModel* sphere;
PlyModel* cylinder;
PlyModel* car;
PlyModel* ground;

int lastIdle;


void init(void)
{	
	//init shaders
	program = InitShader("vshader1.glsl", "fshader1.glsl");
	modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	viewMatrix = glGetUniformLocationARB(program, "projection_matrix");
	vPosition = glGetAttribLocation(program, "vPosition");
	vColor = glGetAttribLocation(program, "vColor");
	vUseColor = glGetUniformLocationARB(program, "useColor");
	vShear = glGetUniformLocationARB(program, "shear");
	vTwist = glGetUniformLocationARB(program, "twist");

	// sets the default color to clear screen
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // black background
	glLineWidth(3);
	//draw lines
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

}

void load_ply(){

	sphere = load_ply("ply_files/sphere.ply");
	cylinder = load_ply("ply_files/cylinder.ply");
	car = load_ply("ply_files/big_porsche.ply");

	point4 groundVerts[4] = { point4(20.0f, -5.0f, -20.0f, 1.0f),
								point4(20.0f, -5.0f, 20.0f, 1.0f),
								point4(-20.0f, -5.0f, 20.0f, 1.0f),
								point4(-20.0f, -5.0f, -20.0f, 1.0f)
							};
	//Make the ground red
	color4 groundColors[4] = {color4(1.0f, 0.0f, 0.0f, 1.0f),
								color4(1.0f, 0.0f, 0.0f, 1.0f),
								color4(1.0f, 0.0f, 0.0f, 1.0f),
								color4(1.0f, 0.0f, 0.0f, 1.0f)};
	ground = new PlyModel(4, groundVerts, groundColors);

	buffer_ply(sphere);
	buffer_ply(cylinder);
	buffer_ply(car);
	buffer_ply(ground);
}

void buffer_ply(PlyModel* model){
	
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);	

	GLuint buffer;
	glGenBuffers(1, &buffer);

	model->vao = vao;
	model->vbo = buffer;
	
	glUseProgram(program);
	int vSize = model->vertexCount*sizeof(vec4);

	glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
	glBufferData(GL_ARRAY_BUFFER, vSize*2, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, model->vertices);
	glBufferSubData(GL_ARRAY_BUFFER, vSize, vSize, model->colors);
	glEnableVertexAttribArray(vPosition);
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vSize));

}

void draw_ply(PlyModel* model){
	glBindVertexArray(model->vao);

	glEnable( GL_DEPTH_TEST );
	
	modelView.pushMatrix();
	modelView.Scale(model->scaleFactor);
	
	glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, Angel::transpose(modelView.plyModel) );
	modelView.popMatrix();
	
	glDrawArrays(GL_LINE_LOOP, 0, model->vertexCount);

	glDisable( GL_DEPTH_TEST );
}

string rewrite_pattern(string str){
	string result = "";
	//printf("rewrite_pattern start: %s\n", str.c_str());
	for(int i = 0; i < str.length(); i++){
		if(str[i] == 'F'){
			result.append(str);
		}else{
			result += str[i];
		}
	}
	//printf("rewrite_pattern end: %s\n", result.c_str());
	return result;
}

void rewrite_lsysStr(){
	for( std::map<char,Rule*>::iterator i = trees.begin(); i != trees.end(); ++i){
		for(int j = 0; j < i->second->iteration; j++){
			i->second->pattern = rewrite_pattern(i->second->pattern);
			printf("Final Pattern: %s\n", i->second->pattern.c_str());
		}
	}
}

void draw_lsys(Rule* rule){
	modelView.pushMatrix();
	//printf("Current Pattern: %s\n", rule->pattern.c_str());

	for(int i = 0; i < rule->pattern.length(); i++){
		char c = rule->pattern[i];
		//printf("Interpreted character: %c", &c);

		modelView.pushMatrix();
		modelView.Scale(0.1f, 0.1f, 0.1f);
		draw_ply(sphere);
		modelView.popMatrix();

		switch(c){
		case 'F':
			modelView.RotateZ(rule->rotation.x);
			modelView.pushMatrix();
			modelView.RotateX(90);
			modelView.Scale(0.1f, 0.1f, rule->len);
			
			draw_ply(cylinder);
			
			modelView.popMatrix();
			modelView.Translate(0,rule->len/10,0);
			break;
		case 'f':
			modelView.Translate(0,0,rule->len);
			break;
		case '+':
			modelView.RotateX(rule->rotation.x);
			break;
		case '-':
			modelView.RotateX(-rule->rotation.x);
			break;
		case '&':
			modelView.RotateY(rule->rotation.y);
			break;
		case '^':
			modelView.RotateY(-rule->rotation.y);
			break;
		case '\\':
			modelView.RotateZ(rule->rotation.z);
			break;
		case '/':
			modelView.RotateZ(-rule->rotation.z);
			break;
		case '|':
			modelView.RotateX(180);
			break;
		case '[':
			modelView.pushMatrix();
			break;
		case ']':
			modelView.popMatrix();
			break;
		}
	}

	modelView.popMatrix();
}

//----------------------------------------------------------------------------
// this is where the drawing should happen
void display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     // clear the window
	
	glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, Angel::transpose(modelView.plyModel) );
	glUniformMatrix4fv( viewMatrix, 1, GL_FALSE, Angel::transpose(proj));
	glUniform1i(vUseColor, useColor);
	glUniform1f(vShear, shear);	
	glUniform1f(vTwist, twist);
	
	glEnable( GL_DEPTH_TEST );
	//rewrite_lsysStr();
	//Draw Car
	draw_ply(car);

	//Draw the forest
	//lsys1.txt
	modelView.pushMatrix();
	modelView.Translate(1.0f, 0.25f, 1.0f);
	draw_lsys(trees['a']);
	modelView.popMatrix();

	//lsys2.txt
	modelView.pushMatrix();
	modelView.Translate(0.0f, 0.25f, 1.0f);
	draw_lsys(trees['b']);
	modelView.popMatrix();

	//Custom file tree
	modelView.pushMatrix();
	modelView.Translate(1.0f, 0.25f, 0.0f);
	draw_lsys(trees['e']);
	modelView.popMatrix();
	
	/*for(int i = 0; i < 5; i++){
		modelView.pushMatrix();
		modelView.Translate(rand()%5, 0.0f, rand()%5);
		draw_lsys(trees['a']);
		modelView.popMatrix();
	}*/

	modelView.pushMatrix();
	modelView.Scale(5.0f, 0.0f, 5.0f);
	draw_ply(ground);
	modelView.popMatrix();

	glDisable( GL_DEPTH_TEST ); 

    glFlush();
	glutSwapBuffers();
}



//----------------------------------------------------------------------------

// keyboard handler
void keyboard( unsigned char key, int x, int y )
{

    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;
	
	case 'f':
		useColor = !useColor;
		break;
    }

	display();
}

//resize handler
void resize(int w, int h){
	width = w;
	height = h;

	glViewport(0, 0, width, height);

	proj = Angel::Perspective((GLfloat)55.0, (GLfloat)width/(GLfloat)height, (GLfloat)0.1, (GLfloat)100.0);
	proj = proj * Angel::LookAt(vec4(0.0f, 0.25f, -1.0f, 1.0f), vec4(0.0f,0.0f,0.0f,1.0f), vec4(0.0f,0.0f,1.0f,1.0f));
}

void idle(){
	int time = glutGet(GLUT_ELAPSED_TIME);

	float delta = (float)(time-lastIdle)/1000.0f;
	lastIdle = time;

	modelView.RotateY(delta * 45.0f);
	
	glutPostRedisplay();
	
}

//----------------------------------------------------------------------------
// entry point
int main( int argc, char **argv )
{
	
	Rule* a = new Rule("lsys_files/lsys1.txt");
	Rule* b = new Rule("lsys_files/lsys2.txt");
	Rule* c = new Rule("lsys_files/lsys3.txt");
	Rule* d = new Rule("lsys_files/lsys4.txt");
	Rule* e = new Rule("lsys_files/lsys_custom.txt");
	
	a->pattern = rewrite_pattern(a->pattern);
	b->pattern = rewrite_pattern(b->pattern);
	c->pattern = rewrite_pattern(c->pattern);
	d->pattern = rewrite_pattern(d->pattern);
	e->pattern = rewrite_pattern(e->pattern);

	//rewrite_lsysStr();

	trees['a'] = a;
	trees['b'] = b;
	trees['c'] = c;
	trees['d'] = d;
	trees['e'] = e;

	cout << a->pattern << endl;
	cout << b->pattern << endl;

	srand(time(NULL));
	// init glut
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
	width = 512;
	height = 512;

	// create window
	// opengl can be incorperated into other packages like wxwidgets, fltoolkit, etc.
    glutCreateWindow( "Sean MacEachern - Homework 3" );

	// init glew
    glewInit();

	init();
    load_ply();
	// assign handlers
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
	glutReshapeFunc(resize);
	int i = 5;
	glutIdleFunc(idle);
	// should add menus
	// add mouse handler
	// add resize window functionality (should probably try to preserve aspect ratio)

	// enter the drawing loop
	// frame rate can be controlled with 
	lastIdle = glutGet(GLUT_ELAPSED_TIME);
	glutMainLoop();

    return 0;
}
