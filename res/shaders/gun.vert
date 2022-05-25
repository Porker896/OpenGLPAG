#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 VP;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal; 
out vec2 TexCoords;

void main()
{ 
    vec4 pos = model * vec4(aPos, 1.0f);

    FragPos = vec3(pos);

    Normal = mat3( transpose( inverse( VP ) ) ) * aNormal;  

    TexCoords = aTexCoords;
    gl_Position = projection * pos;

}
