#version 150

out vec4 outColor;
in vec2 texCoord;
in float age;
//in int tex_choice;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;



void main(void)
{
  vec4 tex_color = texture(tex2, texCoord);
/*
  if(tex_choice == 0.0)
    tex_color = texture(tex1, texCoord);
  if(tex_choice == 1.0)
    tex_color = texture(tex2, texCoord);
  if(tex_choice == 2.0)
    tex_color = texture(tex3, texCoord);
*/
  outColor = vec4(vec3(tex_color), tex_color.w* 1.0/(3*age));
}
