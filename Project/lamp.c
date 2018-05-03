
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"
#include <math.h>
#include "VectorUtils3.h"

GLuint program_lamp, program_cord;
Model *lamp_model, *cord_model;

GLuint current_lamp_index = 0;

mat4 projectionMatrix;

void draw_lamp_model(Model *mod, mat4 mtw, mat4 cam, GLfloat trans_x, GLfloat trans_y, GLfloat trans_z);
void draw_cord_model(Model *mod, mat4 mtw, mat4 cam, GLfloat trans_x, GLfloat trans_y, GLfloat trans_z);
void draw_lamp_with_cord(GLuint program, mat4 mtw, mat4 cam, GLfloat x,GLfloat y,GLfloat z);
void add_lamp(GLfloat x, GLfloat y, GLfloat z);
void initLamp();
void clear_lamps(GLuint program);
void draw_all_lamps(GLuint program, mat4 mtw, mat4 cam);

Point3D lightSourcesColorsArr[] = {
{1.0f,0.95f,0.6f},
{1.0f,0.95f,0.6f},
{1.0f,0.95f,0.6f},
{1.0f,0.95f,0.6f}

};

GLint isAlive[] = {0, 0, 0, 0};


Point3D lightSourcesDirectionsPositions[] = {
{0.0f, 5.0f, 0.0f},
{100.0f, 100.0f,100.0f},
{100.0f, 100.0f,100.0f},
{100.0f, 100.0f,100.0f}
};


void initLamp(mat4 proj_matrix)
{

  program_lamp = loadShaders("lamp.vert", "lamp.frag");
  program_cord = loadShaders("cord.vert", "cord.frag");

  lamp_model = LoadModelPlus("objects/sphere_tc.obj");
  cord_model = LoadModelPlus("objects/cord.obj");

  glUseProgram(program_cord);
	glUniformMatrix4fv(glGetUniformLocation(program_cord, "projMatrix"), 1, GL_TRUE, proj_matrix.m);

  glUseProgram(program_lamp);
	glUniformMatrix4fv(glGetUniformLocation(program_lamp, "projMatrix"), 1, GL_TRUE, proj_matrix.m);

  add_lamp(0, 5, 0);


}


void draw_lamp_model(Model *mod, mat4 mtw, mat4 cam, GLfloat x_trans, GLfloat y_trans, GLfloat z_trans)
{
  mtw = Mult(T(x_trans,y_trans,z_trans),mtw);
  mat4 tot = Mult(cam, mtw);
  glUniformMatrix4fv(glGetUniformLocation(program_lamp, "mdlMatrix"), 1, GL_TRUE, tot.m);
  DrawModel(mod, program_lamp, "inPosition", NULL, NULL);
}

void draw_cord_model(Model *mod, mat4 mtw, mat4 cam, GLfloat x_trans, GLfloat y_trans, GLfloat z_trans)
{
  mtw = Mult(T(x_trans,y_trans,z_trans),mtw);
  mat4 tot = Mult(cam, mtw);
  glUniformMatrix4fv(glGetUniformLocation(program_cord, "mdlMatrix"), 1, GL_TRUE, tot.m);
  DrawModel(mod, program_cord, "inPosition", NULL, NULL);
}

void draw_lamp_with_cord(GLuint program, mat4 mtw, mat4 cam, GLfloat x, GLfloat y, GLfloat z)
{
  glUseProgram(program);

  glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"),
	4, &lightSourcesDirectionsPositions[0].x);
  glUniform1iv(glGetUniformLocation(program, "isAlive"),
	4, isAlive);

	//DRAW LAMP------------------------------------------------
	glUseProgram(program_lamp);
	mtw = S(0.7,0.7,0.7);
	draw_lamp_model(lamp_model, mtw, cam,x,y,z);

	//DRAW CORD------------------------------------------------
	glUseProgram(program_cord);
	mtw = S(1.5,1.5,1.5);

	draw_cord_model(cord_model,mtw, cam, x, y+7, z);

}

void draw_all_lamps(GLuint program, mat4 mtw, mat4 cam)
{
  for (int i = 0; i < 4; i++)
  {
    draw_lamp_with_cord(program, mtw, cam,
      lightSourcesDirectionsPositions[i].x,
      lightSourcesDirectionsPositions[i].y,
      lightSourcesDirectionsPositions[i].z);
  }
}

void add_lamp(GLfloat x, GLfloat y, GLfloat z)
{

  printf("%i\n", current_lamp_index);
  lightSourcesDirectionsPositions[current_lamp_index].x = x;
  lightSourcesDirectionsPositions[current_lamp_index].y = y;
  lightSourcesDirectionsPositions[current_lamp_index].z = z;
  isAlive[current_lamp_index] = 1;
  ++current_lamp_index;

}

void clear_lamps(GLuint program)
{

  for(int i = 1; i<4; i++)
  {
    isAlive[i] = 0;
  }

  for(int i = 1; i<4; i++)
  {
    lightSourcesDirectionsPositions[i].x = 100.0f;
    lightSourcesDirectionsPositions[i].y = 100.0f;
    lightSourcesDirectionsPositions[i].z = 100.0f;
  }

  current_lamp_index = 1;
  glUseProgram(program);
  glUniform1iv(glGetUniformLocation(program, "isAlive"),
	4, isAlive);

}
