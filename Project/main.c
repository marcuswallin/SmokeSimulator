
#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	// Linking hint for Lightweight IDE
	// uses framework Cocoa
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "display.c"
//#include "smoke.c"

GLfloat inner [3];
GLfloat  *floats [1];

void timer(int i)
{
	glutTimerFunc(20, &timer, i);
	glutPostRedisplay();
}

int main(int argc, char **argv)
{/*
  inner[0] = 3.3;
	inner[1] = 2;
	inner[2] = 1.5;
	floats[0] = inner;
  printf("%f\n", inner[0]);
  printf("%f\n", floats[0][2]);
//	intregegers = {inner};
*/
	//smoke s = create_smoke(10.5,3,4,5,6);
	//printf("%d %d %d %d %d \n", (int)s.x, (int)s.y, (int)s.z, (int)s.age, (int)s.vel );
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize (900, 900);
	glutCreateWindow ("Smoke Simulator");
	glutDisplayFunc(display);
	init ();
	glutTimerFunc(20, &timer, 0);

	glutPassiveMotionFunc(mouse);

	glutMainLoop();
	exit(0);
}
