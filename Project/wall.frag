#version 150

out vec4 outColor;
in vec2 texCoord;
uniform sampler2D tex;
uniform mat4 camMatrix;

uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];

uniform bool isDirectional[4];

in vec3 exSurface;
in vec3 exNormal;

void main(void)
{
float diffuse, specular, shade;
vec3 color = vec3(0);
vec3 v = normalize(- exSurface);
vec3 r;
  for (int i = 0; i < 1; i++)
{
		vec3 light = lightSourcesDirPosArr[i];
    vec3 lightView;
		float intensity = 0.5;
		if(isDirectional[i])
		{
				mat3 lightMatrix = mat3(camMatrix);
				lightMatrix = transpose(inverse(lightMatrix));
				lightView = mat3(lightMatrix) * light;

				diffuse = dot(normalize(exNormal), normalize( lightView));
				diffuse = clamp(diffuse, 0, 1);

				r = normalize(reflect(- lightView, normalize(exNormal)));
		}
		else
    {

		   lightView = vec3(camMatrix* vec4(light, 1.0));

		   diffuse = dot(normalize(exNormal), normalize( lightView - exSurface  ));
	     diffuse = clamp(diffuse, 0, 1);

		   r = normalize(reflect( exSurface - lightView, normalize(exNormal)));
			 float to_point = length(lightView - exSurface);

				intensity = 1;
		}
		specular = dot(r,v);
		if (specular > 0.0)
			specular = 1.0 * pow(specular, 50.0);

		specular = clamp(specular, 0, 1);
		shade =  diffuse + specular;

		color = color + (vec3(shade) * lightSourcesColorArr[i]);
  }
    outColor = vec4(color, 1.0) * texture(tex, texCoord);

}
