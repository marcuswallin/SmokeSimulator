
#ifdef __APPLE__
#include <OpenGL/gl3.h>
// Linking hint for Lightweight IDE
// uses framework Cocoa
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"
#include <time.h>
#include <math.h>
#include "VectorUtils3.h"
#include "smoke_emitter.h"
#include "smoke.h"


int roof_height;
GLfloat init_velocity = 4;

void init_smoke_emitters(int scaling_up)
{
  roof_height = scaling_up;
  srand(time(NULL));
  smoke_emitters = malloc (MAX_EMITTERS * sizeof (vec3));

  int nr = 0;
  for(int i = 0; i < nr ; ++i)
  {
    add_smoke_emitter(-20 + 15*i, -scaling_up + 0.1 * scaling_up ,0);
  }

}


bool e_down = false;
bool r_down = false;
void keyboard_interaction(vec3 pos, vec3 look_dir)
{
  if(glutKeyIsDown('e') && !e_down)
  {
    e_down = true;
    add_smoke_emitter(pos.x,-roof_height + 0.1 * roof_height,pos.z);
  }
  else if(!glutKeyIsDown('e') && e_down)
    e_down = false;


  if(glutKeyIsDown('r') && nr_emitters > 0 && !r_down)
  {
    r_down = true;
    remove_smoke_emitter(0);
  }
  else if(!glutKeyIsDown('r') && r_down)
    r_down = false;

}


void spawn_smoke(void)
{
  for(int i = 0; i < nr_emitters; ++i)
  {
    int r_num = rand();
    if(r_num % SPAWN_FREQUENCY == 0)
    {
      float start_x = (float) r_num / RAND_MAX - 0.5;
      float start_y = (float) rand() / RAND_MAX - 0.5;
      vec3 e = smoke_emitters[i];

      add_particle(e.x+start_x*2, e.y+start_y*2, e.z);
    }
  }
}

void smoke_interact_vector_field(int t)
{
  for(int i = 0; i < nr_particles; ++i)
  {
    if(smoke_array[i].age > 8.0)
    {
      remove_particle(i);
      --i;
    }
    else
    {
       interact_vector_field(&smoke_array[i]);
    }
  }
}


void interact_vector_field(smoke *s )
{
   float dist_to_floor = roof_height + s->world_pos.y;

   s->world_pos.y +=  init_velocity / (30 + 5* dist_to_floor);

   float f = 0.05;

   s->world_pos.x += f*((float)rand()/ RAND_MAX - 0.5);
   s->world_pos.z += f*((float)rand()/ RAND_MAX - 0.5);

   s->age += (GLfloat) 1/GROWTH_FACTOR;
   for(int i = 0; i < nr_emitters; ++i)
   {
     //change this
     //float dist = distance_to(*s, smoke_emitters[i]);

    // s->world_pos.y +=  init_velocity / (150 + 20*dist);// (10000 * dist) *
                    //    (roof_height - s->world_pos.y);

  //   s->world_pos.x += (s->world_pos.x - smoke_emitters[i].x)/5000;
  //   s->world_pos.z += (s->world_pos.z - smoke_emitters[i].z)/5000;

   }
}

GLfloat distance_to(smoke s , vec3 emitter)
{
   return sqrt( pow((s.world_pos.x - emitter.x), 2) +
                pow((s.world_pos.y - emitter.y), 2) +
                pow((s.world_pos.z - emitter.z), 2));
}



void add_smoke_emitter(GLfloat x, GLfloat y, GLfloat z)
{
  if(nr_emitters >= MAX_EMITTERS)
    return;

  smoke_emitters[nr_emitters] = SetVector(x,y,z);
  ++nr_emitters;

}

//removes a particle at the designated index
//moves all objects in the list down one step.
void remove_smoke_emitter(int index)
{
  if (index >= nr_emitters || index < 0)
    return;

  for(int i = index; i < nr_emitters - 1; ++i)
  {
    smoke_emitters[i] = smoke_emitters[i + 1];
  }

  --nr_emitters;
}
