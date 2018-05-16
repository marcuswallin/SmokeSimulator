#version 150

in  vec3 inPosition;
in  vec3 inNormal;
in vec2 inTexCoord;
out vec2 texCoord;
out vec3 exNormal;
out vec3 exSurface;


uniform mat4 projMatrix;
uniform mat4 mdlMatrix;

void main(void)
{
	exSurface = (mdlMatrix * vec4(inPosition, 1.0)).xyz;
	gl_Position = projMatrix * mdlMatrix * vec4(inPosition, 1.0);
	mat3 normalMatrix = mat3(mdlMatrix);
	normalMatrix = inverse(normalMatrix);
	normalMatrix = transpose(normalMatrix);
	exNormal = normalMatrix * (-inNormal);
	texCoord = inTexCoord;

}
