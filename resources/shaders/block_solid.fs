#version 330

in vec3 position;
in vec3 normal;
in vec2 texCoord;
in vec4 lightSpacePos;

uniform sampler2D uBlockAtlas;
uniform sampler2D uShadowMap;
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
  float radius;
};

struct DirectionalLightSource {
  vec3 specular;
  vec3 diffuse;
  vec3 direction;
};

const float maxIrradiance = 1.0;

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
  dirLights[0].direction = -normalize(vec3(-40, 80, -50));

  pointLights[0].specular = vec3(2.0);
  pointLights[0].diffuse = vec3(1.2);
  pointLights[0].position = vec3(0, 90, 0);
  pointLights[0].radius = 50;

  // Materials
  materials[0].specularReflection = vec3(0);
  materials[0].diffuseReflection = vec3(0.3);
  materials[0].ambientReflection = vec3(1);
  materials[0].shininess = 32.0;

  Material material = materials[0];
  int lightsCount = 1;

  vec3 irradiance = material.ambientReflection * ambience;
  vec3 cameraDirection = normalize(uCameraPos - position);

  for (int i = 0; i < 1; ++i) {
    // diffuse contribution
    // vec3 lightDirection = normalize(lights[i].position - position);
    vec3 lightDirection = -dirLights[i].direction;
    float reflectance = dot(lightDirection, normal);
    vec3 diffuse = dirLights[i].diffuse * material.diffuseReflection * (max(0, reflectance));

    // specular contribution
    vec3 reflection = normalize(2 * reflectance * normal - lightDirection);

    float gamma = 8;
    float beta = material.shininess / gamma;
    vec3 specularDirection = cross(reflection, cameraDirection);
    float lambda = dot(specularDirection, specularDirection) / 2.0;

    vec3 specular = material.specularReflection * dirLights[i].specular * pow(max(0, 1 - beta * lambda), gamma);
  
    // irradiance += specularTerm;
    // shadow
    // float bias = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.005);  
    float bias = 0.005;

    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(uShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    // float shadow = 0.0;
    // vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
    // for(int x = -1; x <= 1; ++x)
    // {
    //   for(int y = -1; y <= 1; ++y)
    //   {
    //     float pcfDepth = texture(uShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
    //     shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
    //   }    
    // }
    // shadow /= 9.0;

    if(projCoords.z > 1.0)
      shadow = 0.0;
    irradiance += (1 - shadow) * (specular + diffuse);
  }

  for (int i = 0; i < 0; ++i) {
    vec3 lightDirection = pointLights[i].position - position;
    float distanceToLight = length(lightDirection);

    if (distanceToLight > pointLights[i].radius) {
      continue; // Ignore fragments outside the light's radius
    }

    int discreteDistance = int(distanceToLight * 10) / 10;
    float attenuation = max(0.0, 1.0 - float(discreteDistance) / pointLights[i].radius);

    lightDirection = normalize(lightDirection);

    // diffuse contribution
    float reflectance = dot(lightDirection, normal);
    irradiance += attenuation * pointLights[i].diffuse * material.diffuseReflection * (max(0, reflectance));

    // specular contribution
    vec3 reflection = normalize(2 * reflectance * normal - lightDirection);

    float gamma = 8;
    float beta = material.shininess / gamma;
    vec3 specularDirection = cross(reflection, cameraDirection);
    float lambda = dot(specularDirection, specularDirection) / 2.0;

    vec3 specularTerm = material.specularReflection * pointLights[i].specular * pow(max(0, 1 - beta * lambda), gamma);
  
    irradiance += specularTerm;
  }

  if (texColor.a == 0)
    discard;

  FragColor = vec4(irradiance, 1.0f) * texColor;
}
