#version 330

in vec3 position;
in vec3 normal;
in vec2 texCoord;
uniform sampler2D uBlockAtlas;
uniform vec3 uCameraPos;

out vec4 FragColor;

struct Material {
  vec3 specularReflection;
  vec3 diffuseReflection;
  vec3 ambientReflection;
  float shininess;
};

struct PointLightSource {
  vec3 specular;
  vec3 diffuse;
  vec3 position;
};

struct DirectionalLightSource {
  vec3 specular;
  vec3 diffuse;
  vec3 direction;
};

PointLightSource pointLights[1];
DirectionalLightSource dirLights[1];
Material materials[1];

// LightSource lights[3];
// Material material[5];

const vec3 ambience = vec3(0.1);

void main()
{
  vec4 texColor = texture(uBlockAtlas, texCoord);

  const vec3 magenta = vec3(1.0, 0.0, 1.0);
  const float threshold = 0.01;

  if (distance(texColor.rgb, magenta) < threshold) {
    discard;
  }

  // Light sources
  dirLights[0].specular = vec3(1.0);
  dirLights[0].diffuse = vec3(0.6);
  dirLights[0].direction = normalize(vec3(-0.5, -1, -0.3));

  // Materials
  materials[0].specularReflection = vec3(0);
  materials[0].diffuseReflection = vec3(0.3);
  materials[0].ambientReflection = vec3(1);
  materials[0].shininess = 32.0;

  Material material = materials[0];
  int lightsCount = 1;

  vec3 irradiance = material.ambientReflection * ambience;
  vec3 cameraDirection = normalize(uCameraPos - position);

  for (int i = 0; i < lightsCount; ++i) {
    // diffuse contribution
    // vec3 lightDirection = normalize(lights[i].position - position);
    vec3 lightDirection = -dirLights[i].direction;
    float reflectance = dot(lightDirection, normal);
    irradiance += dirLights[i].diffuse * material.diffuseReflection * (max(0, reflectance));

    // specular contribution
    vec3 reflection = normalize(2 * reflectance * normal - lightDirection);

    float gamma = 8;
    float beta = material.shininess / gamma;
    vec3 specularDirection = cross(reflection, cameraDirection);
    float lambda = dot(specularDirection, specularDirection) / 2.0;

    vec3 specularTerm = material.specularReflection * dirLights[i].specular * pow(max(0, 1 - beta * lambda), gamma);
  }

  FragColor = vec4(irradiance, 1.0f) * texColor;
}
