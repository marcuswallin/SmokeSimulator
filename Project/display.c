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
#include "smoke_emitter.c"



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
TextureData wall_tex, floor_tex, roof_tex, smoke_tex_1, smoke_tex_2, smoke_tex_3;
Model *room_model, *floor_model, *roof_model, *billboard_model;
void draw_room_model(Model *mod, mat4 mtw, mat4 cam, GLfloat trans);
void draw_billboard(Model *mod, mat4 mtw, mat4 cam);

int scaling_room_side = 75;
int scaling_room_up = 15;


void init(void)
{

	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//	glCullFace(GL_BACK);

	initControls(scaling_room_side, scaling_room_up);
	cameraPlacement();

	projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 200.0);
	program_room = loadShaders("wall.vert", "wall.frag");
	program_billboard = loadShaders("smoke.vert", "smoke.frag");

	lightToShader(program_room);

	//SMOKE
	glUseProgram(program_billboard);
	glUniformMatrix4fv(glGetUniformLocation(program_billboard, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	init_smoke();
	init_smoke_emitters(scaling_room_up);


	//TEXTURES
	glActiveTexture(GL_TEXTURE0);
	LoadTGATexture("objects/wallpaper_3.tga", &wall_tex);
	glActiveTexture(GL_TEXTURE1);
	LoadTGATexture("objects/floor_2.tga", &floor_tex);
	glActiveTexture(GL_TEXTURE2);
	LoadTGATexture("objects/roof_1.tga", &roof_tex);
	glActiveTexture(GL_TEXTURE3);
	LoadTGATexture("objects/smoke_texture_1.tga", &smoke_tex_1);
	glActiveTexture(GL_TEXTURE4);
	LoadTGATexture("objects/smoke_texture_2.tga", &smoke_tex_2);
	glActiveTexture(GL_TEXTURE5);
	LoadTGATexture("objects/smoke_texture_3.tga", &smoke_tex_3);

	room_model = LoadModelPlus("objects/cube_tc.obj");
	floor_model = LoadModelPlus("objects/floor.obj");
	roof_model = LoadModelPlus("objects/roof.obj");
	billboard_model = LoadModelPlus("objects/billboard.obj");

}


int t = 0;

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 mtw_matrix, cam_matrix;
	cam_matrix = cameraPlacement();
	mtw_matrix = IdentityMatrix();

	//DRAW ROOM----------------------------------------------

	glUseProgram(program_room);
	glUniformMatrix4fv(glGetUniformLocation(program_room, "camMatrix"), 1, GL_TRUE, cam_matrix.m);
	mtw_matrix = S(75,15,75);
	glUniform1i(glGetUniformLocation(program_room, "tex"), 0);
	draw_room_model(room_model, mtw_matrix, cam_matrix, 0);
	glUniform1i(glGetUniformLocation(program_room, "tex"), 1);
	draw_room_model(floor_model, mtw_matrix, cam_matrix, 0.05);
	glUniform1i(glGetUniformLocation(program_room, "tex"), 2);
	draw_room_model(roof_model, mtw_matrix, cam_matrix, -0.1);

	//-------------------------------------------------------

	//DRAW SMOKE---------------------------------------------
	glDisable(GL_DEPTH_TEST);
	glUseProgram(program_billboard);
  vec3 look_dir = get_look_dir();
	vec3 view_pos = get_view_pos();
	spawn_smoke();
	keyboard_interaction(view_pos,look_dir);
	smoke_interact_vector_field(t);
	send_smoke_to_GPU(VectorSub(SetVector(0,0,0), look_dir));
	draw_billboard(billboard_model, mtw_matrix, cam_matrix);
  glEnable(GL_DEPTH_TEST);

/*  if(t % 5 == 0 && t > 50)
  {
		add_particle( 5*cos(t*3.1415/(200) ),-7, 5*sin(t*3.1415/200));
	}
*/
	t++;
	//printf("%i\n", t);
	glutSwapBuffers();
}

void draw_room_model(Model *mod, mat4 mtw, mat4 cam, GLfloat trans)
{
	mtw = Mult(mtw, T(0,trans,0));
	mat4 tot = Mult(cam, mtw);
	glUniformMatrix4fv(glGetUniformLocation(program_room, "mdlMatrix"), 1, GL_TRUE, tot.m);

	DrawModel(mod, program_room, "inPosition", "inNormal", "inTexCoord");
}


void draw_billboard(Model *mod, mat4 mtw, mat4 cam)
{

	glActiveTexture(GL_TEXTURE5);
  glUniform1i(glGetUniformLocation(program_billboard, "tex"), 5);
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
