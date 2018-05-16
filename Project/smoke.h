
#ifndef SMOKE_H
#define SMOKE_H

#define MAX_PARTICLES 5000
GLfloat init_velocity = 0.1;
int nr_particles = 0;

typedef struct smoke
{
  vec3 pos;
  GLfloat age;
  vec3 vel;
  GLfloat tex;
} smoke;

GLuint program_billboard;
smoke *smoke_array;
void add_particle(GLfloat x, GLfloat y, GLfloat z, vec3 init_vel);
void remove_particle(int index);
void init_smoke(void);
void draw_billboard(Model *mod, mat4 mtw, mat4 cam, GLuint billboard_prog);

int partition( smoke a[], int l, int r, vec3 look_vec);
void quick_sort( smoke a[], int l, int r, vec3 look_vec);

GLuint smoke_pos_texdata;

#endif
