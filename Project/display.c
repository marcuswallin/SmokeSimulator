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

#include "lamp.c"



mat4 projectionMatrix;
void lightToShader(GLuint program);


GLuint program_room, program_billboard, program_generators;
TextureData wall_tex, floor_tex, roof_tex, smoke_tex_1, smoke_tex_2,
    smoke_tex_3, bump_tex_1, bump_tex_2, bump_tex_3;
Model *room_model, *floor_model, *roof_model, *billboard_model, *fan_model, *emitter_model;
void draw_room_model(Model *mod, mat4 mtw, mat4 cam, GLfloat trans);

void keyboard_interaction(vec3 pos, vec3 look_dir);

int scaling_room_side = 75;
int scaling_room_up = 15;


void init(void)
{
	printf("%f\n", acos(-1));

	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	initControls(scaling_room_side, scaling_room_up);
	cameraPlacement();

  projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 200.0);

	program_room = loadShaders("shaders/wall.vert", "shaders/wall.frag");
	program_generators = loadShaders("shaders/generators.vert", "shaders/generators.frag");
	program_billboard = loadShaders("shaders/smoke.vert", "shaders/smoke.frag");
  initLamp(projectionMatrix);
	lightToShader(program_room);
	lightToShader(program_generators);
	lightToShader(program_billboard);

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
	glActiveTexture(GL_TEXTURE6);
	LoadTGATexture("objects/texture_1_normal.tga", &bump_tex_1);
	glActiveTexture(GL_TEXTURE7);
	LoadTGATexture("objects/texture_2_normal.tga", &bump_tex_2);
	glActiveTexture(GL_TEXTURE8);
	LoadTGATexture("objects/texture_3_normal.tga", &bump_tex_3);


	room_model = LoadModelPlus("objects/cube_tc.obj");
	floor_model = LoadModelPlus("objects/floor.obj");
	roof_model = LoadModelPlus("objects/roof.obj");
	billboard_model = LoadModelPlus("objects/billboard.obj");
	fan_model = LoadModelPlus("objects/bunnyplus.obj");
	emitter_model =  LoadModelPlus("objects/octagon.obj");
}


int t = 0;

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 mtw_matrix, cam_matrix;
	cam_matrix = cameraPlacement();
	mtw_matrix = IdentityMatrix();


	vec3 look_dir = get_look_dir();
	vec3 view_pos = get_view_pos();
  keyboard_interaction(view_pos,look_dir);
	//DRAW LAMPS----------------------------------------------
  update_light_sources(program_room);
	draw_all_lamps(program_generators, mtw_matrix, cam_matrix);

	//DRAW ROOM----------------------------------------------

	glUseProgram(program_room);
	glUniformMatrix4fv(glGetUniformLocation(program_room, "camMatrix"), 1, GL_TRUE, cam_matrix.m);
	mtw_matrix = S(scaling_room_side,scaling_room_up,scaling_room_side);
	glUniform1i(glGetUniformLocation(program_room, "tex"), 0);
	draw_room_model(room_model, mtw_matrix, cam_matrix, 0);
	glUniform1i(glGetUniformLocation(program_room, "tex"), 1);
	draw_room_model(floor_model, mtw_matrix, cam_matrix, 0.05);
	glUniform1i(glGetUniformLocation(program_room, "tex"), 2);
	draw_room_model(roof_model, mtw_matrix, cam_matrix, -0.1);


	//DRAW SMOKE---------------------------------------------

	glDisable(GL_DEPTH_TEST);
	glUseProgram(program_billboard);
	update_light_sources(program_billboard);
	spawn_smoke();
	smoke_interact_vector_field(t);
	send_smoke_to_GPU(VectorSub(SetVector(0,0,0), look_dir));
	draw_billboard(billboard_model, mtw_matrix, cam_matrix, program_billboard);
  glEnable(GL_DEPTH_TEST);

	//draw fan models-------------------------------------------
	update_light_sources(program_generators);
	glUniformMatrix4fv(glGetUniformLocation(program_generators, "camMatrix"), 1, GL_TRUE, cam_matrix.m);
  draw_fans_and_emitters(program_room, cam_matrix, fan_model, emitter_model);


	t++;

	glutSwapBuffers();
}

void draw_room_model(Model *mod, mat4 mtw, mat4 cam, GLfloat trans)
{
	mtw = Mult(mtw, T(0,trans,0));
	mat4 tot = Mult(cam, mtw);
	glUniformMatrix4fv(glGetUniformLocation(program_room, "mdlMatrix"), 1, GL_TRUE, tot.m);
  glUniformMatrix4fv(glGetUniformLocation(program_room, "camMatrix"), 1, GL_TRUE, cam.m);
	DrawModel(mod, program_room, "inPosition", "inNormal", "inTexCoord");
}


void lightToShader(GLuint program)
{
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

	//glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"),
	//4, &lightSourcesDirectionsPositions[0].x);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"),
	4, &lightSourcesColorsArr[0].x);
	//glUniform1iv(glGetUniformLocation(program, "isDirectional"),
	//4, isDirectional);

}


bool e_down = false;
bool r_down = false;
bool g_down = false;
bool key_q_is_down = false;
void keyboard_interaction(vec3 pos, vec3 look_dir)
{
	if(glutKeyIsDown('q') && !key_q_is_down && (current_lamp_index < 4))
	{
 		key_q_is_down = true;
 		add_lamp(get_view_pos().x,5,get_view_pos().z);
 	}
	else if(!glutKeyIsDown('q') && key_q_is_down)
 	{
 		key_q_is_down = false;
 	}
 	if(glutKeyIsDown('t'))
	{
 		clear_lamps(program_room);
	 }

  if(glutKeyIsDown('e') && !e_down)
  {
    e_down = true;
    add_smoke_emitter(pos.x, pos.y, pos.z, look_dir);
  //  add_smoke_emitter(pos.x,-roof_height + 0.1 * roof_height,pos.z);
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
