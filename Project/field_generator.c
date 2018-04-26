

#ifdef __APPLE__
#include <OpenGL/gl3.h>
// Linking hint for Lightweight IDE
// uses framework Cocoa
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"

#include <math.h>
#include "VectorUtils3.h"

#define MAX_GENERATORS 4

typedef struct field_generator
{
  vec3 world_pos;
  vec3 z_dir;
  vec3 R_dir;
  vec3 phi_dir;
} field_generator;

field_generator *generators;

void init_generators(void)
{
  generators =  malloc (MAX_GENERATORS * sizeof (field_generator));
}


/*
a       -b
b       a-c
c       b


*/
