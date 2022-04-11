#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix; //for instanced rendering

out vec3 FragPos;
out vec3 Normal; 
out vec2 TexCoords;

uniform mat4 VP;
uniform vec3 offset;
uniform int chosenInstance;

void main()
{
    vec3 pos = aPos;

    //if(gl_InstanceID  == chosenInstance)
    //{
     //  pos = aPos + offset;       
    //}

    FragPos = vec3(instanceMatrix * vec4(pos, 1.0));
    Normal = mat3(transpose(inverse(instanceMatrix))) * aNormal;  
    TexCoords = aTexCoords;
    
    gl_Position = VP * vec4(FragPos, 1.0);
}