
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
void add_particle(GLfloat x, GLfloat y, GLfloat z);
void remove_particle(int index);
GLuint smoke_pos_texdata;

//initialises a number of smoke particles.
void init_smoke(void)
{
  glUseProgram(program_billboard);
  int nr = 0;
  smoke_array = malloc (MAX_PARTICLES * sizeof (smoke));

  for(int i = 0; i < nr; ++i)
  {
    add_particle(i-3,-5, 0);
  }
  glUniform1i(glGetUniformLocation(program_billboard, "nrParticles"), nr_particles);

  //init texture data
  glActiveTexture(GL_TEXTURE4);
  glGenTextures(1, &smoke_pos_texdata);
  glBindTexture(GL_TEXTURE_2D, smoke_pos_texdata);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,  nr_particles,1, 0,
    GL_RGBA, GL_FLOAT, &smoke_array[0].x);
    glUniform1i(glGetUniformLocation(program_billboard, "smokePos"), 4);
}

  //sends the smoke array to the GPU
void send_smoke_to_GPU(void)
{
glUseProgram(program_billboard);
glActiveTexture(GL_TEXTURE4);
glUniform1i(glGetUniformLocation(program_billboard, "nrParticles"), nr_particles);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,  nr_particles,1, 0,
  GL_RGBA, GL_FLOAT, &smoke_array[0].x);
  glUniform1i(glGetUniformLocation(program_billboard, "smokePos"), 4);

}

//Adds a smoke particle at the end of the smoke array.
//If the smoke_array is already full, then this will not do anything.
void add_particle(GLfloat x, GLfloat y, GLfloat z)
{
  if(nr_particles >= MAX_PARTICLES)
  return;

  smoke_array[nr_particles].x = x;
  smoke_array[nr_particles].y = y;
  smoke_array[nr_particles].z = z;
  smoke_array[nr_particles].age = 1;

  ++nr_particles;
}


#define GROWTH_FACTOR 1000
#define FLOOR_Y_POS -50
//Iterates over every smoke element and
//applies a movement function on every particle.
void smoke_interact_vector_field(int t)
{

  for (int i = 0; i < nr_particles ; ++i)
  {
    if(smoke_array[i].age > 2.0)
        remove_particle(i);

    smoke tmp = smoke_array[i];
    smoke_array[i].x += tmp.x *tmp.y / 5000;
    smoke_array[i].y += (GLfloat)(tmp.y + FLOOR_Y_POS) / (50*FLOOR_Y_POS) ;
    smoke_array[i].z += tmp.z*tmp.y / 5000;
    smoke_array[i].age += (GLfloat) 1/GROWTH_FACTOR;

  }
}

//removes a particle at the designated index
//moves all objects in the list down one step.
void remove_particle(int index)
{
  if (index >= nr_particles || index < 0)
  return;

  for(int i = index; i < nr_particles - 1; ++i)
  {
    smoke_array[i] = smoke_array[i + 1];
  }

  --nr_particles;
}
