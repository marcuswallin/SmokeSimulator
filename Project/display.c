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

void keyboard_interaction(vec3 pos, vec3 look_dir, vec3 up_vec);
void change_state(int new_state);

int scaling_room_side = 75;
int scaling_room_up = 15;


void init(void)
{
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	initControls(scaling_room_side, scaling_room_up);
	cameraPlacement();

  projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 250.0);

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
	LoadTGATexture("objects/flat_walls.tga", &wall_tex);
	glActiveTexture(GL_TEXTURE1);
	LoadTGATexture("objects/floor_tiling_small.tga", &floor_tex);
	glActiveTexture(GL_TEXTURE2);
	LoadTGATexture("objects/roof_metal_grid.tga", &roof_tex);
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
	fan_model = LoadModelPlus("objects/force_generator.obj");
	emitter_model =  LoadModelPlus("objects/smoke_emitter.obj");
}




void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 mtw_matrix, cam_matrix;
	cam_matrix = cameraPlacement();
	mtw_matrix = IdentityMatrix();

	vec3 look_dir = get_look_dir();
	vec3 view_pos = get_view_pos();
  vec3 up_vec = GetUpVec(look_dir);
  keyboard_interaction(view_pos,look_dir, up_vec);

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
	spawn_smoke(up_vec);
	smoke_interact_vector_field();
	send_smoke_to_GPU(VectorSub(SetVector(0,0,0), look_dir));
	draw_billboard(billboard_model, mtw_matrix, cam_matrix, program_billboard);
  glEnable(GL_DEPTH_TEST);

	//draw fan models-------------------------------------------
	update_light_sources(program_generators);
	glUniformMatrix4fv(glGetUniformLocation(program_generators, "camMatrix"), 1, GL_TRUE, cam_matrix.m);
  draw_fans_and_emitters(program_room, cam_matrix, fan_model, emitter_model);

  //DRAW LAMPS----------------------------------------------
  update_light_sources(program_room);
  draw_all_lamps(program_generators, mtw_matrix, cam_matrix);

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

	glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"),
	6, &lightSourcesColorsArr[0].x);

}


bool e_down = false;
bool mouse_down = false;
int player_mode = 1;
bool key_is_down = false;
//lets the user interact with the simulation
void keyboard_interaction(vec3 pos, vec3 look_dir, vec3 up_vec)
{
  vec3 dist = ScalarMult(look_dir, 8);

  vec3 d = ScalarMult(up_vec, 3);

//CHECK IF CHANGE OF STATES-----------------------------
  if(glutKeyIsDown('1') && !key_is_down)
  {
    change_state(1);
  }
  else if(glutKeyIsDown('2') &&  !key_is_down)
  {
    change_state(2);
  }
  else if(glutKeyIsDown('3') &&  !key_is_down)
  {
    change_state(3);
  }
  else if(glutKeyIsDown('4') && !key_is_down)
  {
    change_state(4);
  }
  else if((!glutKeyIsDown('1') || !glutKeyIsDown('2') ||
          !glutKeyIsDown('3') || !glutKeyIsDown('4')) &&
  key_is_down)
    key_is_down = false;


  //CHANGE POSITION OF CURRENTLY HELD OBJECT--------------------------------
  remove_moving_objects();
  remove_moving_lamp();
  if(player_mode == 2)
  {
    add_lamp(pos.x + look_dir.x * 6 ,pos.y + look_dir.y * 6 - 3 , pos.z + look_dir.z * 6, 2);
  }
  else if(player_mode == 3)
  {
    add_smoke_emitter(pos.x+dist.x-d.x, pos.y +dist.y-d.y, pos.z+dist.z-d.z, up_vec, true);
  }
  else if(player_mode == 4)
  {
    add_field_generator(pos.x+dist.x-d.x, pos.y +dist.y-d.y, pos.z+dist.z-d.z, look_dir, true);
  }


//If mouse is pressed add new object---------------------------------------
  if(glutMouseIsDown(1) && !mouse_down)
  {
    mouse_down = true;
    if (player_mode == 2)
    {
      add_lamp(pos.x + look_dir.x * 6 ,pos.y + look_dir.y * 6 - 3 , pos.z + look_dir.z * 6, 1);
    }
    if (player_mode == 3)
    {
      add_smoke_emitter(pos.x+dist.x-d.x, pos.y +dist.y-d.y, pos.z+dist.z-d.z, up_vec, false);
    }
    else if(player_mode == 4)
    {
      add_field_generator(pos.x+dist.x-d.x, pos.y +dist.y-d.y, pos.z+dist.z-d.z, look_dir, false);
    }
  }
  else if(!glutMouseIsDown(1)  && mouse_down)
    mouse_down = false;


  //remove object ie (erase) is pressed---------------------------------------
  if(glutKeyIsDown('e') && !e_down)
  {
    e_down = true;
    if (player_mode == 2 && current_lamp_index > 1)
    {
      remove_lamp(0);
    }
    if (player_mode == 3 && nr_emitters > 1)
    {
      remove_smoke_emitter(0);
    }
    else if(player_mode == 4 && nr_generators > 1)
    {
      remove_generator(0);
    }
  }
  else if(!glutKeyIsDown('e')  && e_down)
    e_down = false;

}

//changes the player_mode to the new state
//also removes old objects.
void change_state(int new_state)
{
    player_mode = new_state;
    key_is_down = true;
    remove_moving_objects();
    remove_moving_lamp();
}
