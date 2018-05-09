
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
//#include "smoke_emitter.h"
#include "smoke.h"

//initialises a number of smoke particles.
void init_smoke(void)
{
  glUseProgram(program_billboard);
  //int nr = 0;
  smoke_array = malloc (MAX_PARTICLES * sizeof (smoke));

  add_particle(-3,0,0,SetVector(0,0,0));
  add_particle(0,0,0,SetVector(0,0,0));
  add_particle(3,0,0,SetVector(0,0,0));
/*
  for(int i = 0; i < nr; ++i)
  {
    add_particle( 5*cos(i*3.1415/(nr/2) ),0, 5*sin(i*3.1415/(nr/2)));
  }

  if(nr_particles > 0)
    quick_sort(smoke_array, 0, nr_particles - 1, SetVector(1,0,0));
*/

  glUniform1i(glGetUniformLocation(program_billboard, "nrParticles"), nr_particles);

  //init texture data--------------------------------------------------
  glActiveTexture(GL_TEXTURE15);
  glGenTextures(1, &smoke_pos_texdata);
  glBindTexture(GL_TEXTURE_2D, smoke_pos_texdata);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 2, nr_particles, 0,
    GL_RGBA, GL_FLOAT, &smoke_array[0].pos.x);
  glUniform1i(glGetUniformLocation(program_billboard, "smokePos"), 15);
}


  //sends the smoke array to the GPU
  void send_smoke_to_GPU(vec3 look_dir)
  {
    glUseProgram(program_billboard);
    glActiveTexture(GL_TEXTURE15);
  //  glUniform1i(glGetUniformLocation(program_billboard, "smokePos"), 15);
    glUniform1i(glGetUniformLocation(program_billboard, "nrParticles"), nr_particles );
  //  quick_sort(smoke_array, 0, nr_particles-1, look_dir);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 2, nr_particles, 0,
      GL_RGBA, GL_FLOAT, &smoke_array[0].pos.x);


  }

//Adds a smoke particle at the end of the smoke array.
//If the smoke_array is already full, then this will not do anything.
void add_particle(GLfloat x, GLfloat y, GLfloat z, vec3 init_vel)
{
  if(nr_particles >= MAX_PARTICLES)
    return;

  vec3 pos = SetVector(x,y,z);
  smoke_array[nr_particles].pos = pos;
  smoke_array[nr_particles].age = 0.1;
  smoke_array[nr_particles].vel = init_vel;
  smoke_array[nr_particles].tex = (float)(nr_particles % 3);
  ++nr_particles;

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


void draw_billboard(Model *mod, mat4 mtw, mat4 cam, GLuint billboard_prog)
{
	glActiveTexture(GL_TEXTURE3);
  glUniform1i(glGetUniformLocation(program_billboard, "tex1"), 3);
	glActiveTexture(GL_TEXTURE4);
  glUniform1i(glGetUniformLocation(program_billboard, "tex2"), 4);
	glActiveTexture(GL_TEXTURE5);
  glUniform1i(glGetUniformLocation(billboard_prog, "tex3"), 5);
  glActiveTexture(GL_TEXTURE6);
  glUniform1i(glGetUniformLocation(billboard_prog, "bump1"), 6);
  glActiveTexture(GL_TEXTURE7);
  glUniform1i(glGetUniformLocation(billboard_prog, "bump2"), 7);
  glActiveTexture(GL_TEXTURE8);
  glUniform1i(glGetUniformLocation(billboard_prog, "bump3"), 8);
	glUniformMatrix4fv(glGetUniformLocation(billboard_prog, "camMatrix"), 1, GL_TRUE, cam.m);
	glUniformMatrix4fv(glGetUniformLocation(billboard_prog, "mtwMatrix"), 1, GL_TRUE, mtw.m);
	DrawModelInstanced(mod, billboard_prog, "inPosition", NULL, "inTexCoord", nr_particles);
}


//sorts the smoke array according to siew-plane coordinates!
//look dir should be the direction we want to sort in. Can be minus
void quick_sort( smoke a[], int l, int r, vec3 look_dir)
{
   int j;

   if( l < r )
   {
   	// divide and conquer
        j = partition( a, l, r, look_dir);
       quick_sort( a, l, j-1, look_dir);
       quick_sort( a, j+1, r, look_dir);
   }

}



int partition( smoke a[], int l, int r, vec3 look_dir) {
   int i, j;
   smoke t;
   GLfloat pivot = DotProduct(a[l].pos, look_dir);
   i = l; j = r+1;

   while( 1)
   {
   	do ++i; while( DotProduct(a[i].pos, look_dir) <= pivot && i <= r );
   	do --j; while( DotProduct(a[j].pos, look_dir) > pivot );
   	if( i >= j ) break;
   	t = a[i]; a[i] = a[j]; a[j] = t;
   }
   t = a[l]; a[l] = a[j]; a[j] = t;
   return j;
}
