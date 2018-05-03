#version 150

in vec3 inPosition;
in vec2 inTexCoord;

uniform sampler2D smokePos;

out float age;
out vec2 texCoord;
out float tex_choice;

uniform int nrParticles;
uniform mat4 projMatrix;
uniform mat4 camMatrix;
uniform mat4 mtwMatrix;

void main(void)
{
//  vec3 pos = data_SSBO[0];
  mat4 mtw_modified = mtwMatrix;
  float instance = float(gl_InstanceID);
  float nr = float(nrParticles);

  float v = instance / nr + 1.0/1000000;
  //float vel_and_tex = (instance * 2 + 1) / nr + 1.0/1000000;
  vec4 texVal  = texture(smokePos, vec2(0, v));
  vec4 vel_and_tex_vec = texture(smokePos, vec2(0.5, v));
  tex_choice = vel_and_tex_vec.w;

	mtw_modified[3][0] = mtwMatrix[3][0] + texVal.x;
	mtw_modified[3][1] = mtwMatrix[3][1] + texVal.y;
	mtw_modified[3][2] = mtwMatrix[3][2] + texVal.z;

  mat4 tot = camMatrix * mtw_modified;

  tot[0] = vec4(texVal.w, 0.0 , 0.0, 0.0);
  tot[1] = vec4(0.0, texVal.w , 0.0, 0.0);
  tot[2] = vec4(0.0, 0.0 , texVal.w, 0.0);

	gl_Position = projMatrix * tot *  vec4(inPosition, 1.0);

  age = texVal.w;
	texCoord = inTexCoord;

}
