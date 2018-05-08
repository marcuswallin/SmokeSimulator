
#ifndef LAMP_H
#define LAMP_H

void draw_lamp_model(Model *mod, mat4 mtw, mat4 cam, GLfloat trans_x, GLfloat trans_y, GLfloat trans_z);
void draw_cord_model(Model *mod, mat4 mtw, mat4 cam, GLfloat trans_x, GLfloat trans_y, GLfloat trans_z);
void draw_lamp_with_cord(GLuint program, mat4 mtw, mat4 cam, GLfloat x,GLfloat y,GLfloat z);
void add_lamp(GLfloat x, GLfloat y, GLfloat z);
void initLamp();
void clear_lamps(GLuint program);
void draw_all_lamps(GLuint program, mat4 mtw, mat4 cam);


GLuint program_lamp, program_cord;
Model *lamp_model, *cord_model;

GLuint current_lamp_index = 0;

mat4 projectionMatrix;


Point3D lightSourcesColorsArr[] = {
{1.0f,0.95f,0.6f},
{1.0f,0.95f,0.6f},
{1.0f,0.95f,0.6f},
{1.0f,0.95f,0.6f}

};

GLint isAlive[] = {0, 0, 0, 0};


Point3D lightSourcesDirectionsPositions[] = {
{0.0f, 5.0f, 0.0f},
{100.0f, 100.0f,100.0f},
{100.0f, 100.0f,100.0f},
{100.0f, 100.0f,100.0f}
};


#endif
