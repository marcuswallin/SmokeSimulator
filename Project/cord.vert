#version 150

in  vec3 inPosition;
//in vec2 inTexCoord;
//out vec2 texCoord;


// NY
uniform mat4 projMatrix;
uniform mat4 mdlMatrix;

void main(void)
{
	gl_Position = projMatrix * mdlMatrix * vec4(inPosition, 1.0);
	//texCoord = inTexCoord;

}
