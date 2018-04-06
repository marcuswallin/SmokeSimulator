#version 150

in  vec3 inPosition;
in vec2 inTexCoord;

out vec2 texCoord;

uniform vec4 smoke_pos[10];
uniform mat4 projMatrix;
uniform mat4 camMatrix;
uniform mat4 mtwMatrix;

void main(void)
{
  vec4 pos = smoke_pos[gl_InstanceID];
  mat4 mtw_modified = mtwMatrix;

	mtw_modified[3][0] = mtwMatrix[3][0];
	mtw_modified[3][1] = mtwMatrix[3][1] + gl_InstanceID;
	mtw_modified[3][2] = mtwMatrix[3][2] ;

  mat4 tot = camMatrix * mtw_modified;

  tot[0] = vec4(1.0, 0.0 , 0.0, 0.0);
  tot[1] = vec4(0.0, 1.0 , 0.0, 0.0);
  tot[2] = vec4(0.0, 0.0 , 1.0, 0.0);

	gl_Position = projMatrix * tot *  vec4(inPosition, 1.0);

	texCoord = inTexCoord;

}
