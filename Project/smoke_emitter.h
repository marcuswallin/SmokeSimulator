
#ifndef SMOKE_EMITTER_H
#define SMOKE_EMITTER_H

#define MAX_EMITTERS 6
#define SPAWN_FREQUENCY 4
#define GROWTH_FACTOR 100
#define MAX_AGE 8.0
#define FRICTION 0.98
#define Y_VELOCITY 0.002
#define FIELD_STRENGTH 0.05
#define SPREAD 0.002
int nr_emitters = 0;
int nr_generators = 0;
int roof_height;

#define MAX_GENERATORS 4

typedef struct field_generator
{
  vec3 pos;
  vec3 dir;
  mat3 T;
  mat3 inverseT;
} field_generator;

field_generator *generators;
field_generator *smoke_emitters;

mat3 get_trans_matrix(vec3 look_dir);
vec3 get_coord_new_system(vec3 pos, int gen_index);
vec3 get_vel_new_system(vec3 vel, int gen_index);
void field_generator_interaction(smoke *s);

void init_generators(void);
void init_smoke_emitters(int);
void add_smoke_emitter( GLfloat x, GLfloat y, GLfloat z, vec3 look_dir);
//void add_smoke_emitter(GLfloat x, GLfloat y, GLfloat z);
//void remove_smoke_emitter(int index);
void roof_interaction(smoke  *s);

void interact_vector_field(smoke *s );
void smoke_interact_vector_field(int t);

GLfloat distance_to(smoke s , vec3 emitter);

void keyboard_interaction(vec3 pos, vec3 look_dir);

void add_field_generator(GLfloat x, GLfloat y, GLfloat z, vec3 look_dir);
void remove_generator(int index);



#endif
