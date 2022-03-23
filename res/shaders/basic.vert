#version 430 core

layout (location = 0) in vec3 aPos;

uniform mat4 VP;
uniform mat4 model;

void main()
{ 
    gl_Position = VP *  model * vec4(aPos, 1.0);

}