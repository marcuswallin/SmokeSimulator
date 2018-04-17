
#ifdef __APPLE__
#include <OpenGL/gl3.h>
// Linking hint for Lightweight IDE
// uses framework Cocoa
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"
#include <math.h>
#include "VectorUtils3.h"
#include "loadobj.h"

#define MAX_PARTICLES 2000
int nr_particles = 0;

typedef struct smoke
{
   float x, y, z, age;
} smoke;

GLuint program_billboard;
smoke *smoke_array;
void add_smoke(GLfloat x, GLfloat y, GLfloat z, GLfloat age);

//initialises a number of smoke particles.
void init_smoke(void)
{
   int nr = 20;
   smoke_array = malloc (MAX_PARTICLES * sizeof (smoke));

   for(int i = 0; i < nr; ++i)
   {
      add_smoke(5 + 10*sin(3.1415 * i / nr), i, 0,0);

   }

}
void send_smoke_to_GPU(void)
{

  glUniform1i(glGetUniformLocation(program_billboard, "nrParticles"), nr_particles);
  GLuint texID;
  //change this
  glActiveTexture(GL_TEXTURE4);
	glGenTextures(1, &texID);
  glBindTexture(GL_TEXTURE_2D, texID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,  nr_particles,1, 0,
      GL_RGBA, GL_FLOAT, &smoke_array[0].x);

  glUniform1i(glGetUniformLocation(program_billboard, "smokePos"), 4);

}

//change this to insert the smnoke in the array.
void add_smoke(GLfloat x, GLfloat y, GLfloat z, GLfloat age)
{

	 smoke_array[nr_particles].x = x;
	 smoke_array[nr_particles].y = y;
   smoke_array[nr_particles].z = z;
   smoke_array[nr_particles].age = age;

   ++nr_particles;

}

int ti = 0;
void smoke_interact_vector_field(void)
{
   ++ti;
   for (int i = 0; i < nr_particles ; ++i)
   {
      smoke t = smoke_array[i];
      float d =  sqrt(t.x*t.x + t.z*t.z) ;
    //  float angle = acos(t.x /d);

    //  float new_angle = angle + 0.01;
  //    printf("%f\n", new_angle);

      float new_angle = ti * 0.01;
      smoke_array[i].x = d*cos(new_angle );
      smoke_array[i].z = d*sin(new_angle );

   }
}
