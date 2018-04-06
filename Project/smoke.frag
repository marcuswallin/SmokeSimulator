#version 150

out vec4 outColor;
in vec2 texCoord;
uniform sampler2D tex;

uniform vec4 smoke_pos[10];

void main(void)
{
    if(smoke_pos[0].x == 3)
       outColor = vec4(1.0, 0.0, 0.5,1.0);
    else
       outColor = texture(tex, texCoord);
}
