#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	// Linking hint for Lightweight IDE
	// uses framework Cocoa
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include "cameracontrol.c"
#include "smoke.c"


mat4 projectionMatrix;
void lightToShader(GLuint program);

Point3D lightSourcesColorsArr[] = { {1.0f, 1.0f, 1.0f},
{0.0f, 1.0f, 0.0f},
{1.0f, 1.0f, 1.0f},
{1.0f, 1.0f, 1.0f} };

GLint isDirectional[] = {0,0,1,1};

Point3D lightSourcesDirectionsPositions[] = { {0.0f, 9.0f, 0.0f},
{8.0f, 20.0f, 40.0f},
{-1.0f, 1.0f, 0.0f},
{0.0f, 1.0f, 1.0f} };

GLuint program_room, program_billboard;
TextureData wall_tex, floor_tex, roof_tex, smoke_tex;
Model *room_model, *floor_model, *roof_model, *billboard_model;
void draw_room_model(Model *mod, mat4 mtw, mat4 cam, GLuint trans);
void draw_billboard(Model *mod, mat4 mtw, mat4 cam);


void init(void)
{

   glClearColor(0.2,0.2,0.5,0);
	 glEnable(GL_DEPTH_TEST);
	 glDisable(GL_CULL_FACE);
   initControls();

   projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 200.0);
   program_room = loadShaders("wall.vert", "wall.frag");
	 program_billboard = loadShaders("smoke.vert", "smoke.frag");


   lightToShader(program_room);


//SMOKE
   glUseProgram(program_billboard);
	 glUniformMatrix4fv(glGetUniformLocation(program_billboard, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
   init_smoke();
	// send_smoke_to_GPU();
	// convert_to_array();
		// send_smoke_to_GPU(billboard_model);

 	 /*for (int i = 0; i< 4*SMOKE_MAX_SIZE; ++i)
 	 {
 		 printf("%f\n", smoke_as_floats[i]);
 	 }
*/
	// init_smoke_particles();
	 //lightToShader(program_billboard);


	 glActiveTexture(GL_TEXTURE0);
   LoadTGATexture("objects/wallpaper_3.tga", &wall_tex);
   glActiveTexture(GL_TEXTURE1);
	 LoadTGATexture("objects/floor_2.tga", &floor_tex);
   glActiveTexture(GL_TEXTURE2);
	 LoadTGATexture("objects/roof_1.tga", &roof_tex);
	 glActiveTexture(GL_TEXTURE3);
   LoadTGATexture("objects/maskros512.tga", &smoke_tex);

	 send_smoke_to_GPU();


	 room_model = LoadModelPlus("objects/cube_tc.obj");
	 floor_model = LoadModelPlus("objects/floor.obj");
   roof_model = LoadModelPlus("objects/roof.obj");
	 billboard_model = LoadModelPlus("objects/billboard.obj");

}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 mtw_matrix, cam_matrix;
  //DRAW ROOM----------------------------------------------
//  glUseProgram(program_room);
	cam_matrix = cameraPlacement();

	mtw_matrix = IdentityMatrix();

	glUseProgram(program_billboard);

	glUniform1i(glGetUniformLocation(program_billboard, "tex"), 3);

	smoke_interact_vector_field();
	send_smoke_to_GPU();
	draw_billboard(billboard_model, mtw_matrix, cam_matrix);

	glUseProgram(program_room);
	glUniformMatrix4fv(glGetUniformLocation(program_room, "camMatrix"), 1, GL_TRUE, cam_matrix.m);
	mtw_matrix = S(30,15,30);
	glUniform1i(glGetUniformLocation(program_room, "tex"), 0);
  draw_room_model(room_model, mtw_matrix, cam_matrix, 0);
  glUniform1i(glGetUniformLocation(program_room, "tex"), 1);
  draw_room_model(floor_model, mtw_matrix, cam_matrix, 0.05);
  glUniform1i(glGetUniformLocation(program_room, "tex"), 2);
  draw_room_model(roof_model, mtw_matrix, cam_matrix, -0.1);

  //-------------------------------------------------------

  glutSwapBuffers();
}

void draw_room_model(Model *mod, mat4 mtw, mat4 cam, GLuint trans)
{
  mtw = Mult(mtw, T(0,trans,0));
  mat4 tot = Mult(cam, mtw);
  glUniformMatrix4fv(glGetUniformLocation(program_room, "mdlMatrix"), 1, GL_TRUE, tot.m);

  DrawModel(mod, program_room, "inPosition", "inNormal", "inTexCoord");
}


void draw_billboard(Model *mod, mat4 mtw, mat4 cam)
{

	glUniformMatrix4fv(glGetUniformLocation(program_billboard, "camMatrix"), 1, GL_TRUE, cam.m);
  glUniformMatrix4fv(glGetUniformLocation(program_billboard, "mtwMatrix"), 1, GL_TRUE, mtw.m);


	DrawModelInstanced(mod, program_billboard, "inPosition", NULL, "inTexCoord", nr_particles);
}


void lightToShader(GLuint program)
{
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

	glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"),
	4, &lightSourcesDirectionsPositions[0].x);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"),
	4, &lightSourcesColorsArr[0].x);
	glUniform1iv(glGetUniformLocation(program, "isDirectional"),
	4, isDirectional);

}
