
#ifdef __APPLE__
#include <OpenGL/gl3.h>
// Linking hint for Lightweight IDE
// uses framework Cocoa
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"
#include <math.h>
#include "VectorUtils3.h"


vec3 p,l;
GLfloat old_theta = M_PI;
GLfloat delta_theta = 0.0f;
GLfloat old_phi = M_PI /2;
GLfloat delta_phi = 0.0f;
int x = 0;
int y = 0;
mat4 trans, rot;
int room_size_roof, room_size_walls;
vec3 get_view_pos(void);


void initControls(int size_walls, int size_roof)
{
  room_size_roof = size_roof;
  room_size_walls = size_walls;
  p = SetVector(0.0f,  0.0f, 1.0f);
  l = SetVector(0.0f, 0.0f, 0.0f);
  trans = IdentityMatrix();
  glutHideCursor();
}

//called by mouse().
//calculates the new rotation angles of the camera in polar coordinates.
void cameraRotation()
{

  int distance_middle_x = x - (int)glutGet(GLUT_WINDOW_WIDTH) / 2;
  int distance_middle_y = y - (int)glutGet(GLUT_WINDOW_HEIGHT) / 2;
  int k = 3000;
  delta_theta = (GLfloat) distance_middle_x / k;
  delta_phi = (GLfloat) distance_middle_y / k;

}

//looks where the mouse is placed.
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

  vec3 v = SetVector(0.0f, 1.0f, 0.0f);
  v = Normalize(v);

  mat4 old_trans = trans;
  glutPassiveMotionFunc(mouse);
  old_theta -= delta_theta;
  old_phi += delta_phi;

  if (old_phi > M_PI || old_phi < 0)
    old_phi -= delta_phi;

  GLfloat f = 0.2;

  float z_val = f*sin(old_phi)*cos(old_theta);
  float x_val = f*sin(old_phi)*sin(old_theta);
  float y_val = f*cos(old_phi);

  if(glutKeyIsDown('w'))
    trans = Mult(trans, T(-x_val, y_val,-z_val));
  if(glutKeyIsDown('a'))
    trans = Mult(trans, T(-f*cos(old_theta),0,+f*sin(old_theta)));
  if(glutKeyIsDown('s'))
    trans = Mult(trans, T(x_val,-y_val,z_val));
  if(glutKeyIsDown('d'))
    trans = Mult(trans, T(+f*cos(old_theta),0,-f*sin(old_theta)));

  vec3 move = MultVec3(trans, p);
  if(move.x < -room_size_walls + 1 || move.x > room_size_walls - 1 ||
  move.y < -room_size_roof + 1 || move.y > room_size_roof - 2 ||
  move.z < -room_size_walls + 1  || move.z > room_size_walls - 1  )
    trans = old_trans;


  rot = Mult(Ry(old_theta), Rx(M_PI /2 - old_phi));

  return  lookAtv(get_view_pos(),
  MultVec3(Mult(trans, Mult( T(p.x, p.y, p.z),
  Mult(rot, T(-p.x, -p.y, -p.z)))), l) ,
  v);
}

//returns the world coordinates of the viewer.
vec3 get_view_pos(void)
{
  return MultVec3(trans, p);
}

//returns the look direction of the viewer.
vec3 get_look_dir(void)
{
  vec3 tmp_l = MultVec3(Mult(trans, Mult( T(p.x, p.y, p.z),
                                          Mult(rot, T(-p.x, -p.y, -p.z)))), l);
  vec3 tmp_p = get_view_pos();
  vec3 look_dir = Normalize(VectorSub(tmp_l,tmp_p));
  return look_dir;
}
