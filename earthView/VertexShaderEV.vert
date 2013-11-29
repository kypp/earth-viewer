#version 330 core

layout(location=0) in vec3 position;
out float ht;

uniform vec3 par;
uniform vec2 segment; //x - szer, y - wys
uniform mat4 MVP;

void main(){ 
    gl_Position.xyz =  position.xyz;
	gl_Position.x +=  1200*(segment.y-par.y);
	gl_Position.y +=  1200*(par.x-segment.x);
	//gl_Position.y = cos(gl_Position.y);
	gl_Position.y *=  -1;
	//gl_Position.xy *= par.z;
	//gl_Position.z = -position.z/8000;
    gl_Position.w = 1.0;
	gl_Position.xy /= 500/par.z;
	ht = position.z;
	gl_Position.z = -ht/30000;
	//gl_Position.x *= cos(segment.y/180);
	//gl_Position *= MVP;
}