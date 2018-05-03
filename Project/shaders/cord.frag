#version 150

out vec4 outColor;
//in vec2 texCoord;
uniform sampler2D tex;

void main(void)
{
    outColor = vec4(0.1,0.1,0.1,1.0);
    //texCoord = vec2(1.0);

}
