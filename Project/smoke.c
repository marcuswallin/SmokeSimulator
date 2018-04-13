
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

#define SMOKE_MAX_SIZE 10


typedef struct smoke
{
   GLfloat x, y, z, age, vel;
} smoke;

GLuint program_billboard;
smoke *smoke_array;
float* smoke_as_floats;

//initialises a number of smoke particles.
void init_smoke(void)
{
  smoke_as_floats = (float*)malloc(4*SMOKE_MAX_SIZE*sizeof(float));
   smoke_array = malloc (SMOKE_MAX_SIZE * sizeof (smoke));

   for(int i = 0; i < SMOKE_MAX_SIZE; ++i)
   {
      smoke_array[i].x = 2+10*sin((3.1415 * i ) / SMOKE_MAX_SIZE  );
      smoke_array[i].y = 0;
      smoke_array[i].z = i;
      smoke_array[i].age = 0;
   }
}

//converts the array of smoke structs to an array of array of GLfloat.
//should be updated to only convert the existing particles,
// with a global variable.
void convert_to_array(void)
{

  for (int i = 0; i< SMOKE_MAX_SIZE; ++i)
  {

     smoke_as_floats[i*4 + 0] = smoke_array[i].x;
     smoke_as_floats[i*4 + 1] = smoke_array[i].y;
     smoke_as_floats[i*4 + 2] = smoke_array[i].z;
     smoke_as_floats[i*4 + 3] = smoke_array[i].age;

  //   printf("%f\n", smoke_as_floats[i][0]);
  }
}

void send_smoke_to_GPU(void)
{
  GLuint texID;
  //change this
  glActiveTexture(GL_TEXTURE4);
	glGenTextures(1, &texID);
  glBindTexture(GL_TEXTURE_2D, texID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,  SMOKE_MAX_SIZE,1, 0,
      GL_RGBA, GL_FLOAT, &smoke_as_floats[0]);

  glUniform1i(glGetUniformLocation(program_billboard, "smokePos"), 4);

}
/*
void send_smoke_to_GPU(Model *m)
{
  GLint loc;
  //buffer
  unsigned int smoke_buffer;

  glBindVertexArray(m->vao);

  glGenBuffers(1, &smoke_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, smoke_buffer);

  loc = glGetAttribLocation(program_billboard, "smokePos");
  glBufferData(GL_ARRAY_BUFFER, SMOKE_MAX_SIZE*sizeof(GLfloat)*4, smoke_as_floats, GL_STATIC_DRAW);
  glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0,0);
  glEnableVertexAttribArray(loc);
}*/
//glBufferSubData
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
