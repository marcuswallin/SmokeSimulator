
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
}

void cameraRotation()
{
  
	int distance_middle_x = x - (int)glutGet(GLUT_WINDOW_HEIGHT) / 2;
	int distance_middle_y = y - (int)glutGet(GLUT_WINDOW_WIDTH) / 2;

	delta_theta = (GLfloat) distance_middle_x / 20000;
	delta_phi = (GLfloat) distance_middle_y / 20000;
}


void mouse(int x1,int y1)
{
	x=x1;
	y=y1;
	cameraRotation();
}


mat4 cameraPlacement()
{

	p = SetVector(0.0f,  5.0f, 10.0f);
	vec3 l = SetVector(0.0f, 5.0f, 0.0f);
	vec3 v = SetVector(0.0f, 1.0f, 0.0f);
	v = Normalize(v);

	glutPassiveMotionFunc(mouse);
	old_theta -= delta_theta;
	old_phi -= delta_phi;
  if (fabs((float)old_phi) > 3.1415 / 2)
      old_phi += delta_phi;

//  mat4 old_trans = trans;
	if(glutKeyIsDown('w'))
	  trans = Mult(trans, T(-0.2*sin(old_theta),0.2*sin(old_phi),-0.2*cos(old_theta)));
	if(glutKeyIsDown('a'))
	  trans = Mult(trans, T(-0.2*cos(old_theta),0,+0.2*sin(old_theta)));
	if(glutKeyIsDown('s'))
  	trans = Mult(trans, T(+0.2*sin(old_theta),-0.2*sin(old_phi),+0.2*cos(old_theta)));
	if(glutKeyIsDown('d'))
	  trans = Mult(trans, T(+0.2*cos(old_theta),0,-0.2*sin(old_theta)));

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