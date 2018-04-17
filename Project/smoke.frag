#version 150

out vec4 outColor;
in vec2 texCoord;
uniform sampler2D tex;



void main(void)
{

       vec4 tex = texture(tex, texCoord);
       outColor = vec4(vec3(tex), 0.5);
}
