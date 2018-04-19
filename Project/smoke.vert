#version 150

in vec3 inPosition;
in vec2 inTexCoord;

uniform sampler2D smokePos;

out float age;
out vec2 texCoord;

uniform int nrParticles;
uniform mat4 projMatrix;
uniform mat4 camMatrix;
uniform mat4 mtwMatrix;

void main(void)
{
//  vec3 pos = data_SSBO[0];
  mat4 mtw_modified = mtwMatrix;

  float instance = float(gl_InstanceID);
  float v = (instance) / nrParticles + 1.0/1000000;
  vec4 texVal  = texture(smokePos, vec2(v, 0));

	mtw_modified[3][0] = mtwMatrix[3][0] + texVal.x;
	mtw_modified[3][1] = mtwMatrix[3][1] + texVal.y ;
	mtw_modified[3][2] = mtwMatrix[3][2] + texVal.z;

  mat4 tot = camMatrix * mtw_modified;

  tot[0] = vec4(texVal.w, 0.0 , 0.0, 0.0);
  tot[1] = vec4(0.0, texVal.w , 0.0, 0.0);
  tot[2] = vec4(0.0, 0.0 , texVal.w, 0.0);


	gl_Position = projMatrix * tot *  vec4(inPosition, 1.0);

  age = texVal.w;
	texCoord = inTexCoord;

}
