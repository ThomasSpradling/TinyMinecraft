#version 330 core

in vec3 fragColor;
out vec4 FragColor;

float near = 0.1; 
float far  = 256.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}


void main()
{    
  FragColor = vec4(fragColor, 1.0f);
}