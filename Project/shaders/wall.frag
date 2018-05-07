#version 150

out vec4 outColor;
in vec2 texCoord;
uniform sampler2D tex;
uniform mat4 camMatrix;

uniform vec3 lightSourcesDirPosArr[6];
uniform vec3 lightSourcesColorArr[6];

uniform bool isAlive[6];

in vec3 exSurface;
in vec3 exNormal;

void main(void)
{
  float diffuse, specular, shade;
  vec3 color = vec3(0);
  vec3 v = normalize(- exSurface);
  vec3 r;
  for (int i = 0; i < 6; i++)
  {

    vec3 light = lightSourcesDirPosArr[i];
    vec3 lightView;
    lightView = vec3(camMatrix* vec4(light, 1.0));


    diffuse = dot(normalize(exNormal), normalize( lightView - exSurface  ));
    diffuse = clamp(diffuse, 0, 1);

    r = normalize(reflect( exSurface - lightView, normalize(exNormal)));
    float to_point = length(lightView - exSurface);

    specular = dot(r,v);
    if (specular > 0.0)
    {
      specular = 1.0 * pow(specular, 50.0);
    }

    specular = clamp(specular, 0, 1);

    //shade =  diffuse + specular;

    if(isAlive[i])
    {
      shade =  diffuse + specular;
    }
    else
    {
        shade =  0;
    }
    float energy = 150 / (0.6* pow((1 + length( lightView - exSurface)), 2));
    float intensity = clamp(energy, 0 ,1);

    color = color + intensity * (vec3(shade) * lightSourcesColorArr[i]);
  }
  outColor = vec4(color, 1.0) * texture(tex, texCoord);

}
