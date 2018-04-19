
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

#define MAX_PARTICLES 5000
int nr_particles = 0;

typedef struct smoke
{
  vec3 world_pos;
  GLfloat age;
//  vec3 view_pos;
//  GLfloat rot;
} smoke;

GLuint program_billboard;
smoke *smoke_array;
void add_particle(GLfloat x, GLfloat y, GLfloat z);
void remove_particle(int index);

int partition( smoke a[], int l, int r, vec3 look_vec);
void quick_sort( smoke a[], int l, int r, vec3 look_vec);
//GLfloat* convert_to_array(void);
GLuint smoke_pos_texdata;

//initialises a number of smoke particles.
void init_smoke(void)
{
  glUseProgram(program_billboard);
  int nr =0;
  smoke_array = malloc (MAX_PARTICLES * sizeof (smoke));

  for(int i = 0; i < nr; ++i)
  {
    add_particle( 5*cos(i*3.1415/(nr/2) ),0, 5*sin(i*3.1415/(nr/2)));
  //   add_particle( -5 + i,0, -15);
  }

  printf("\n %i \n", nr_particles);
  if(nr_particles > 0)
    quick_sort(smoke_array, 0, nr_particles - 1, SetVector(1,0,0));

  for (int i = 0; i < nr_particles ; ++i)
  {   //printf("%f %f %f \n", smoke_array[i].world_pos.x,
      //   smoke_array[i].world_pos.y, smoke_array[i].world_pos.z);
  }

  glUniform1i(glGetUniformLocation(program_billboard, "nrParticles"), nr_particles);

  //init texture data
  glActiveTexture(GL_TEXTURE4);
  glGenTextures(1, &smoke_pos_texdata);
  glBindTexture(GL_TEXTURE_2D, smoke_pos_texdata);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

//  GLfloat* a = convert_to_array();

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,  nr_particles,1, 0,
    GL_RGBA, GL_FLOAT, &smoke_array[0].world_pos.x);
  glUniform1i(glGetUniformLocation(program_billboard, "smokePos"), 4);
}


  //sends the smoke array to the GPU
  void send_smoke_to_GPU(vec3 look_dir)
  {
    glUseProgram(program_billboard);
    glActiveTexture(GL_TEXTURE4);
    glUniform1i(glGetUniformLocation(program_billboard, "nrParticles"), nr_particles);
    quick_sort(smoke_array, 0, nr_particles-1, look_dir);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,  nr_particles,1, 0,
      GL_RGBA, GL_FLOAT, &smoke_array[0].world_pos.x);
    glUniform1i(glGetUniformLocation(program_billboard, "smokePos"), 4);

  }

//Adds a smoke particle at the end of the smoke array.
//If the smoke_array is already full, then this will not do anything.
void add_particle(GLfloat x, GLfloat y, GLfloat z)
{
  if(nr_particles >= MAX_PARTICLES)
    return;

  vec3 pos = SetVector(x,y,z);
  smoke_array[nr_particles].world_pos = pos;
  smoke_array[nr_particles].age = 1;
//  vec3 view_pos = SetVector(x+10,y,z);
//  smoke_array[nr_particles].view_pos = view_pos;
//  smoke_array[nr_particles].view_pos.x += 10;
  //smoke_array[nr_particles].rot = 0;

  ++nr_particles;
  printf("%i\n", nr_particles);
}


#define GROWTH_FACTOR 500
#define FLOOR_Y_POS -50
//Iterates over every smoke element and
//applies a movement function on every particle.
void smoke_interact_vector_field(int t)
{

  for (int i = 0; i < nr_particles ; ++i)
  {
    if(smoke_array[i].age > 4.0)
        remove_particle(i);

    smoke tmp = smoke_array[i];
    smoke_array[i].world_pos.x += tmp.world_pos.x *tmp.world_pos.y / 5000;
    smoke_array[i].world_pos.y += (GLfloat)(tmp.world_pos.y +
      FLOOR_Y_POS) / (50*FLOOR_Y_POS) ;
    smoke_array[i].world_pos.z += tmp.world_pos.z*tmp.world_pos.y / 5000;
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
   GLfloat pivot = DotProduct(a[l].world_pos, look_dir);
   i = l; j = r+1;

   while( 1)
   {
   	do ++i; while( DotProduct(a[i].world_pos, look_dir) <= pivot && i <= r );
   	do --j; while( DotProduct(a[j].world_pos, look_dir) > pivot );
   	if( i >= j ) break;
   	t = a[i]; a[i] = a[j]; a[j] = t;
   }
   t = a[l]; a[l] = a[j]; a[j] = t;
   return j;
}
