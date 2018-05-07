#version 150

out vec4 outColor;
in vec2 texCoord;
in float age;
in float tex_choice;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D bump1;
uniform sampler2D bump2;
uniform sampler2D bump3;

uniform mat4 camMatrix;

uniform vec3 lightSourcesDirPosArr[6];
uniform vec3 lightSourcesColorArr[6];
uniform bool isAlive[6];

in vec3 exSurface;

void main(void)
{
  vec4 tex_color;// = texture(tex2, texCoord);
  vec3 exNormal;
  if(tex_choice < 0.5)
  {
    tex_color = texture(tex1, texCoord);
    exNormal = vec3(texture(bump1, texCoord));
  }
  else if(tex_choice < 1.5)
  {
    tex_color = texture(tex2, texCoord);
    exNormal = vec3(texture(bump2, texCoord));
  }
  else if(tex_choice  < 2.5)
  {
    tex_color = texture(tex3, texCoord);
    exNormal = vec3(texture(bump3, texCoord));
  }
  else
  {
    tex_color = vec4(1);
     exNormal = vec3(0.0, 0.0, 1.0);
  }

  float diffuse, specular, shade;
  vec3 color = vec3(0);
  vec3 v = normalize( -exSurface);
  vec3 r;
  for (int i = 0; i < 6; i++)
    {

      if(!isAlive[i])
        continue;

      vec3 light = lightSourcesDirPosArr[i];
      vec3 lightView;
      lightView = vec3(camMatrix* vec4(light, 1.0));

      float energy = 150 / (0.6* pow((1 + length( lightView - exSurface)), 2));
      float intensity = clamp(energy, 0 ,1);
      diffuse = dot(normalize(exNormal), normalize(  lightView -exSurface   ));
      shade = clamp((diffuse + 1.2)/2, 0, 1);

      color = color + intensity * (vec3(shade) * lightSourcesColorArr[i]);
     }

  outColor = vec4(vec3(tex_color)*color, tex_color.w* 1.0/(3*age));
}
