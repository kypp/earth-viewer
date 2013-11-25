#version 330 core

layout(location=0) in vec3 position;
out float ht;

uniform mat4 MVP;

void main(){ 
	gl_Position.xyz = position;
	gl_Position.w = 1.0;

	gl_Position *= MVP;

	ht = 1000;
}