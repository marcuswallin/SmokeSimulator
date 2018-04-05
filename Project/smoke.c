
#ifdef __APPLE__
#include <OpenGL/gl3.h>
// Linking hint for Lightweight IDE
// uses framework Cocoa
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"
#include <math.h>
#include "VectorUtils3.h"

const int SMOKE_MAX_SIZE = 10;

stats_t *smoke_array = malloc (SMOKE_MAX_SIZE * sizeof (smoke));

typedef struct smoke
{
   GLfloat x, y, z, age, vel;
} smoke;

smoke create_smoke(GLfloat x, GLfloat y, GLfloat z, GLfloat age, GLfloat vel)
{
   smoke s;
	 s.x = x;
	 s.y = y;
	 s.z = z;
	 s.age = age;
	 s.vel = vel;
	 return s;
}

void init_smoke_particles(void)
{
   for(int i = 0; i < SMOKE_MAX_SIZE; ++i)
   {
      smoke_array[i].x = 20*sin((3.1415 * i )/ SMOKE_MAX_SIZE  );
      smoke_array[i].y = 0;
      smoke_array[i].z = 0;
      smoke_array[i].age = 0;
      smoke_array[i].vel = 0;

   }
}
