
#ifdef __APPLE__
#include <OpenGL/gl3.h>
// Linking hint for Lightweight IDE
// uses framework Cocoa
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"
#include <math.h>
#include "VectorUtils3.h"


vec3 p;
GLfloat old_theta = 0.0f;
GLfloat delta_theta = 0.0f;
GLfloat old_phi = 0.0f;
GLfloat delta_phi = 0.0f;
int x = 0;
int y = 0;
mat4 trans;

void initControls()
{
  trans = IdentityMatrix();
  glutHideCursor();
}

void cameraRotation()
{

  int distance_middle_x = x - (int)glutGet(GLUT_WINDOW_WIDTH) / 2;
  int distance_middle_y = y - (int)glutGet(GLUT_WINDOW_HEIGHT) / 2;
  //  printf("%i %i\n", distance_middle_x, distance_middle_y);
  int k = 3000;
  delta_theta = (GLfloat) distance_middle_x / k;
  delta_phi = (GLfloat) distance_middle_y / k;

  //  printf("%f %f\n", delta_theta, delta_phi);
}

void mouse(int x1,int y1)
{
  int center_x = (int)glutGet(GLUT_WINDOW_WIDTH) / 2;
  int center_y = (int)glutGet(GLUT_WINDOW_HEIGHT) / 2;
  x=x1;
  y=y1;
  cameraRotation();
  glutWarpPointer(center_x, center_y);
  glutPostRedisplay();
}

//returns a matrix containing the world to view matrix.
mat4 cameraPlacement()
{

  p = SetVector(0.0f,  -5.0f, 20.0f);
  vec3 l = SetVector(0.0f, -5.0f, 0.0f);
  vec3 v = SetVector(0.0f, 1.0f, 0.0f);
  v = Normalize(v);

  glutPassiveMotionFunc(mouse);
  old_theta -= delta_theta;
  old_phi -= delta_phi;
  if (fabs((float)old_phi) > 3.1415 / 2)
    old_phi += delta_phi;

  GLfloat f = 0.2;

  if(glutKeyIsDown('w'))
    trans = Mult(trans, T(-f*sin(old_theta),f*sin(old_phi),-f*cos(old_theta)));
  if(glutKeyIsDown('a'))
    trans = Mult(trans, T(-f*cos(old_theta),0,+f*sin(old_theta)));
  if(glutKeyIsDown('s'))
    trans = Mult(trans, T(+f*sin(old_theta),-f*sin(old_phi),+f*cos(old_theta)));
  if(glutKeyIsDown('d'))
    trans = Mult(trans, T(+f*cos(old_theta),0,-f*sin(old_theta)));

  /*vec3 move = MultVec3(trans, p);
  if(move.x < -25 || move.x > 25 ||
  move.y < -5 || move.y > 5 ||
  move.z < -25 || move.z > 25 )
  trans = old_trans;
  */

  mat4 rot = Mult(Ry(old_theta), Rx(old_phi));

  return  lookAtv(MultVec3(trans, p),
  MultVec3(Mult(trans, Mult( T(p.x, p.y, p.z),
  Mult(rot, T(-p.x, -p.y, -p.z)))), l) ,
  v);
}

vec3 get_view_pos(void)
{
  return MultVec3(trans, p);
}
