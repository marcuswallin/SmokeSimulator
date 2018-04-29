
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


void init_smoke_emitters(int scaling_up)
{
  init_generators();

  int particle_estimation = (int) (MAX_EMITTERS * 1.0 / SPAWN_FREQUENCY *
     (MAX_AGE - 1)*GROWTH_FACTOR);
  printf("\nApproximate max nr of particles: %i\n \n", particle_estimation );
  roof_height = scaling_up;
  srand(time(NULL));
  smoke_emitters = malloc (MAX_EMITTERS * sizeof (vec3));

  int nr = 1;
  for(int i = 0; i < nr ; ++i)
  {
    add_smoke_emitter(-20 + 15*i, -scaling_up + 0.1 * scaling_up ,-20);
  }

}


bool e_down = false;
bool r_down = false;
bool g_down = false;
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


  if(glutKeyIsDown('g') && !g_down)
  {
    g_down = true;
    add_field_generator(pos.x,pos.y,pos.z, look_dir);
  }
  else if(!glutKeyIsDown('g') && g_down)
    g_down = false;



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
    if(smoke_array[i].age > MAX_AGE)
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

//manipulates the speed of the particles.
//which is later added to the particle's position.
void interact_vector_field(smoke *s )
{
   float dist_to_floor = roof_height + s->pos.y;

   float f = 0.002;
   s->vel.x += f*((float)rand()/ RAND_MAX - 0.5);
   s->vel.z += f*((float)rand()/ RAND_MAX - 0.5);

   roof_interaction(s);
   field_generator_interaction(s);
   s->age += (GLfloat) 1/GROWTH_FACTOR;
  // for(int i = 0; i < nr_generators; ++i)
   //{

  //   vec3 rel_pos_gen = get_coord_new_system(s->pos, i);


     //change this
     //float dist = distance_to(*s, smoke_emitters[i]);

    // s->world_pos.y +=  init_velocity / (150 + 20*dist);// (10000 * dist) *
                    //    (roof_height - s->world_pos.y);

  //   s->world_pos.x += (s->world_pos.x - smoke_emitters[i].x)/5000;
  //   s->world_pos.z += (s->world_pos.z - smoke_emitters[i].z)/5000;

  // }
  //change this
   s->pos.x += s->vel.x*0.95;
   s->pos.y += s->vel.y*0.95;
   s->pos.z += s->vel.z*0.95;

}

//
void roof_interaction(smoke  *s)
{
  GLfloat diff = roof_height - s->pos.y;
  s->vel.y = s->vel.y*(1-(float)1/(25*diff));
}

void field_generator_interaction(smoke *s)
{
//  printf("%i\n", nr_generators);

  for(int index = 0; index < nr_generators; index++)
  {
     vec3 rel_pos_gen = get_coord_new_system(s->pos, index);
    // printf("%f %i\n", rel_pos_gen.x, index);
    if(rel_pos_gen.z < 0 || rel_pos_gen.z > 30 ||
      sqrt(pow(rel_pos_gen.x,2) + pow(rel_pos_gen.y,2)) > 10)
      continue;


    vec3 rel_speed = get_vel_new_system(s->vel, index);
  //  printf("%f\n", rel_speed.z);
    rel_speed.z += (float)1/(1+3*rel_pos_gen.z);

    vec3 new_speed_old_sys = MultMat3Vec3(generators[index].inverseT, rel_speed);
    s->vel.x = new_speed_old_sys.x;
    s->vel.y = new_speed_old_sys.y;
    s->vel.z = new_speed_old_sys.z;

  }
}

GLfloat distance_to(smoke s , vec3 other)
{
   return sqrt( pow((s.pos.x - other.x), 2) +
                pow((s.pos.y - other.y), 2) +
                pow((s.pos.z - other.z), 2));
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


//GENERATORS------------------------------------------------------------------------------
//input must be normalized
mat3 get_trans_matrix(vec3 look_dir)
{
  mat3 trans;
  //adding look dir as z-drection
  trans.m[6] = look_dir.x;
  trans.m[7] = look_dir.y;
  trans.m[8] = look_dir.z;

  vec3 ydir = Normalize(SetVector(look_dir.y,  look_dir.z - look_dir.x , -look_dir.y));
  trans.m[3] = ydir.x;
  trans.m[4] = ydir.y;
  trans.m[5] = ydir.z;

  vec3 xdir = Normalize(CrossProduct(ydir, look_dir));
  trans.m[0] = xdir.x;
  trans.m[1] = xdir.y;
  trans.m[2] = xdir.z;

  return trans;
}

vec3 get_coord_new_system(vec3 pos, int gen_index)
{
   vec3 rel_pos = VectorSub(pos, generators[gen_index].pos);
   return MultMat3Vec3(generators[gen_index].T, rel_pos);
}

vec3 get_vel_new_system(vec3 vel, int gen_index)
{
   return MultMat3Vec3(generators[gen_index].T, vel);
}




void init_generators(void)
{
  generators =  malloc (MAX_GENERATORS * sizeof (field_generator));
}


void add_field_generator(GLfloat x, GLfloat y, GLfloat z, vec3 look_dir)
{
  if(nr_generators >= MAX_GENERATORS)
    return;

  generators[nr_generators].pos = SetVector(x,y,z);
  generators[nr_generators].T = get_trans_matrix(look_dir);
  generators[nr_generators].inverseT = InvertMat3(generators[nr_generators].T);
  printf("%f %f %f \n", generators[nr_generators].pos.x, generators[nr_generators].pos.y, generators[nr_generators].pos.z);
  ++nr_generators;

}

//removes a generator at the designated index
//moves all objects in the list down one step.
void remove_generator(int index)
{
  if (index >= nr_generators || index < 0)
    return;

  for(int i = index; i < nr_generators - 1; ++i)
  {
    generators[i] = generators[i + 1];
  }

  --nr_generators;
}
