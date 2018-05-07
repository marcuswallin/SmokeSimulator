
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
void add_lamp(GLfloat x, GLfloat y, GLfloat z, int);
void initLamp();
void clear_lamps(GLuint program);
void draw_all_lamps(GLuint program, mat4 mtw, mat4 cam);
void remove_lamp(int index);

Point3D lightSourcesColorsArr[] = {
{0.95f,0.95f,0.7f},
{0.95f,0.95f,0.7f},
{0.95f,0.95f,0.7f},
{0.95f,0.95f,0.7f},
{0.95f,0.95f,0.7f},
{0.95f,0.95f,0.7f}

};

GLint isAlive[] = {0, 0, 0, 0, 0, 0};


Point3D lightSourcesDirectionsPositions[] = {
{100.0f, 100.0f,100.0f},
{100.0f, 100.0f,100.0f},
{100.0f, 100.0f,100.0f},
{100.0f, 100.0f,100.0f},
{100.0f, 100.0f,100.0f},
{100.0f, 100.0f,100.0f}
};


void initLamp(mat4 proj_matrix)
{

  program_lamp = loadShaders("shaders/lamp.vert", "shaders/lamp.frag");
  program_cord = loadShaders("shaders/cord.vert", "shaders/cord.frag");

  lamp_model = LoadModelPlus("objects/sphere_tc.obj");
  cord_model = LoadModelPlus("objects/cord.obj");

  glUseProgram(program_cord);
	glUniformMatrix4fv(glGetUniformLocation(program_cord, "projMatrix"), 1, GL_TRUE, proj_matrix.m);

  glUseProgram(program_lamp);
	glUniformMatrix4fv(glGetUniformLocation(program_lamp, "projMatrix"), 1, GL_TRUE, proj_matrix.m);

//  add_lamp(0, 5, 0,1);


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


	//DRAW LAMP------------------------------------------------
	glUseProgram(program_lamp);
	mtw = S(0.7,0.7,0.7);
	draw_lamp_model(lamp_model, mtw, cam,x,y,z);

	//DRAW CORD------------------------------------------------
	glUseProgram(program_cord);
	mtw = S(5,4,5);

	draw_cord_model(cord_model,mtw, cam, x, y+20, z);

}

void update_light_sources(GLuint program)
{
  glUseProgram(program);

  glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"),
  6, &lightSourcesDirectionsPositions[0].x);
  glUniform1iv(glGetUniformLocation(program, "isAlive"),
  6, isAlive);

}

void draw_all_lamps(GLuint program, mat4 mtw, mat4 cam)
{
  for (int i = 0; i < current_lamp_index; i++)
  {
    if(isAlive[i])
    {
    draw_lamp_with_cord(program, mtw, cam,
      lightSourcesDirectionsPositions[i].x,
      lightSourcesDirectionsPositions[i].y,
      lightSourcesDirectionsPositions[i].z);
    }
  }
}

//need to reset isAlive to zero.
//is_alive should be either 1 for permantent and 2 for moving
void add_lamp(GLfloat x, GLfloat y, GLfloat z, int is_alive)
{
  printf("%i %i %i %i %i %i\n", isAlive[0],isAlive[1], isAlive[2], isAlive[3], isAlive[4], isAlive[5]);
  if((current_lamp_index >= 6))
    return;

  //printf("%i\n", current_lamp_index);
  lightSourcesDirectionsPositions[current_lamp_index].x = x;
  lightSourcesDirectionsPositions[current_lamp_index].y = y;
  lightSourcesDirectionsPositions[current_lamp_index].z = z;
  isAlive[current_lamp_index] = is_alive;
  ++current_lamp_index;

}

//these two does not work properly
void remove_moving_lamp(void)
{
  for(int i = 0; i < 6; ++i)
  {
    //2 means temporary
    if(isAlive[i] == 2)
    {
     remove_lamp(i);
    }
  }
}
//same
void remove_lamp(int index)
{
  if (index >= current_lamp_index || index < 0)
    return;

  for(int j = index; j < current_lamp_index -1 ; ++j)
  {
    isAlive[j] = isAlive[j+1];
    lightSourcesDirectionsPositions[j] = lightSourcesDirectionsPositions[j+1];
  }
  --current_lamp_index;
}

void clear_lamps(GLuint program)
{

  for(int i = 1; i<6; i++)
  {
    isAlive[i] = 0;
  }

  for(int i = 1; i<6; i++)
  {
    lightSourcesDirectionsPositions[i].x = 100.0f;
    lightSourcesDirectionsPositions[i].y = 100.0f;
    lightSourcesDirectionsPositions[i].z = 100.0f;
  }

  current_lamp_index = 1;
  glUseProgram(program);
  glUniform1iv(glGetUniformLocation(program, "isAlive"),
	6, isAlive);

}
