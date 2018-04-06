
#ifdef __APPLE__
#include <OpenGL/gl3.h>
// Linking hint for Lightweight IDE
// uses framework Cocoa
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"
#include <math.h>
#include "VectorUtils3.h"

#define SMOKE_MAX_SIZE 10


typedef struct smoke
{
   GLfloat x, y, z, age, vel;
} smoke;

GLuint program_billboard;
smoke *smoke_array;
GLfloat *smoke_as_floats[SMOKE_MAX_SIZE];

//initialises a number of smoke particles.
void init_smoke(void)
{
   smoke_array = malloc (SMOKE_MAX_SIZE * sizeof (smoke));

   for(int i = 0; i < SMOKE_MAX_SIZE; ++i)
   {
      smoke_array[i].x = 3.0;//10*sin((3.1415 * i ) / SMOKE_MAX_SIZE  );
      smoke_array[i].z = i;
   }
}

//converts the array of smoke structs to an array of array of GLfloat.
//should be updated to only convert the existing particles,
// with a global variable.
void convert_to_array(void)
{
  for (int i = 0; i< SMOKE_MAX_SIZE; ++i)
  {
     GLfloat* convert = malloc(4*sizeof(GLfloat));
     convert[0] = smoke_array[i].x;
     convert[1] = smoke_array[i].y;
     convert[2] = smoke_array[i].z;
     convert[3] = smoke_array[i].age;
     smoke_as_floats[i] = convert;
  //   printf("%f\n", smoke_as_floats[i][0]);
  }
}

void send_smoke_to_GPU(void)
{
   glUniform4fv(glGetUniformLocation(program_billboard, "smoke_pos"),
     SMOKE_MAX_SIZE, &smoke_as_floats[0][0]);

}

//change this to insert the smnoke in the array.
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
