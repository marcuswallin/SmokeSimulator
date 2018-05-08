
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
  smoke_emitters = malloc (MAX_EMITTERS * sizeof (field_generator));

  int nr = 1;
  for(int i = 0; i < nr ; ++i)
  {
    add_smoke_emitter(-20 + 15*i, -scaling_up + 0.1 * scaling_up ,-20,
       ScalarMult(SetVector(0,1,0), INIT_VEL));
  }

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
      vec3 e = smoke_emitters[i].pos;
      vec3 init_vel = ScalarMult(smoke_emitters[i].dir, INIT_VEL);
      add_particle(e.x+start_x*2, e.y+start_y*2, e.z, init_vel);
    }
  }
}

void draw_fans_and_emitters(GLuint program, mat4 cam_mat, Model *fan, Model *emitter)
{
  mat4 rot;
  mat4 tot;
  field_generator curr;
  GLint loc = glGetUniformLocation(program, "mdlMatrix");

  for(int i = 0; i < nr_generators; ++i)
  {
     curr = generators[i];
     rot = RotateToAxis(curr.dir);

     tot = Mult(cam_mat, Mult(T(curr.pos.x, curr.pos.y, curr.pos.z), rot));
     glUniformMatrix4fv(loc, 1, GL_TRUE, tot.m);
     DrawModel(fan, program, "inPosition", "inNormal", "inTexCoord");
  }

  for(int t = 0; t < nr_emitters; ++t)
  {
    curr = smoke_emitters[t];
    rot = RotateToAxis(curr.dir);
    tot = Mult(cam_mat, Mult(T(curr.pos.x, curr.pos.y, curr.pos.z), rot));
    glUniformMatrix4fv(loc, 1, GL_TRUE, tot.m);
    DrawModel(emitter, program, "inPosition", "inNormal", "inTexCoord");
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
  float f;

if(s->age < MAX_AGE-2)
{
   f = 0.001*(powf(s->age, 2)+s->age);
 }
 else
 {
   f = 0.001/s->age;
 }

   s->vel.x += f*((float)rand()/ RAND_MAX - 0.5);
   s->vel.z += f*((float)rand()/ RAND_MAX - 0.5);

   roof_interaction(s);
   field_generator_interaction(s);
   s->age += (GLfloat) 1/GROWTH_FACTOR;

   //apply friction
   if (s->age < MAX_AGE -6)
   {
   s->vel.x = s->vel.x*FRICTION;
   s->vel.y = s->vel.y*FRICTION;
   s->vel.z = s->vel.z*FRICTION;
   }
   else
   {
     s->vel.x = s->vel.x*(FRICTION+0.004);
     s->vel.y = s->vel.y*(FRICTION+0.004);
     s->vel.z = s->vel.z*(FRICTION+0.004);
   }
   //update position
   s->pos.x += s->vel.x;
   s->pos.y += s->vel.y;
   s->pos.z += s->vel.z;

}

//
void roof_interaction(smoke  *s)
{
  GLfloat diff = roof_height - s->pos.y;
  s->vel.y = (Y_VELOCITY + s->vel.y)*(1-(float)1/(50*diff));
}

void field_generator_interaction(smoke *s)
{


  for(int index = 0; index < nr_generators; index++)
  {
     vec3 rel_pos_gen = get_coord_new_system(s->pos, index);

    if(rel_pos_gen.z < 0 || rel_pos_gen.z > 80 ||
      sqrt(pow(rel_pos_gen.x,2) + pow(rel_pos_gen.y,2)) > 10)
      continue;

    vec3 rel_speed = get_vel_new_system(s->vel, index);

    //particles get a push in z-dir
    rel_speed.z += FIELD_STRENGTH/(5+rel_pos_gen.z);

    //Particle gets a radial velocity
    GLfloat rel_dist = sqrt( pow(rel_pos_gen.x, 2) + pow(rel_pos_gen.y, 2));
    GLfloat spread_coeff = SPREAD / (10 + rel_dist);
    rel_speed.x += rel_pos_gen.x*spread_coeff;
    rel_speed.y += rel_pos_gen.y*spread_coeff;

    //convert back to world coordinates
    vec3 new_speed_old_sys = MultMat3Vec3(generators[index].inverseT, rel_speed);
    s->vel.x = new_speed_old_sys.x;
    s->vel.y = new_speed_old_sys.y;
    s->vel.z = new_speed_old_sys.z;

    //makes particles age quicker when blown by a fan
    if(rel_pos_gen.z < 1)
      s->age += 1/(GROWTH_FACTOR);
    else
      s->age += 1/(GROWTH_FACTOR*rel_pos_gen.z);


  }
}

GLfloat distance_to(smoke s , vec3 other)
{
   return sqrt( pow((s.pos.x - other.x), 2) +
                pow((s.pos.y - other.y), 2) +
                pow((s.pos.z - other.z), 2));
}


void add_smoke_emitter( GLfloat x, GLfloat y, GLfloat z, vec3 look_dir)
{
  if(nr_emitters >= MAX_EMITTERS)
    return;

  smoke_emitters[nr_emitters].pos = SetVector(x,y,z);
  vec3 rot_axis = CrossProduct(look_dir, SetVector(0,1,0));
  mat4 rot_mat = ArbRotate(rot_axis, 3.1415/2);
  smoke_emitters[nr_emitters].dir = Normalize(MultVec3(rot_mat,look_dir));
  smoke_emitters[nr_emitters].T = get_trans_matrix(look_dir);
  smoke_emitters[nr_emitters].inverseT = InvertMat3(smoke_emitters[nr_emitters].T);
  printf("%f %f %f \n", smoke_emitters[nr_emitters].pos.x, smoke_emitters[nr_emitters].pos.y, smoke_emitters[nr_emitters].pos.z);
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
  generators[nr_generators].dir = look_dir;
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
