
#ifndef SMOKE_EMITTER_H
#define SMOKE_EMITTER_H

#define MAX_EMITTERS 6
#define SPAWN_FREQUENCY 10
#define GROWTH_FACTOR 1000
int nr_emitters = 0;
vec3 *smoke_emitters;



void init_smoke_emitters(int);
void add_smoke_emitter(GLfloat x, GLfloat y, GLfloat z);
void remove_smoke_emitter(int index);

void interact_vector_field(smoke *s );
void smoke_interact_vector_field(int t);

GLfloat distance_to(smoke s , vec3 emitter);

void keyboard_interaction(vec3 pos, vec3 look_dir);



#endif
