#version 330 core

layout(location=0) in vec3 position;
out float ht;

uniform vec3 par;
uniform vec2 segment; //x - szer, y - wys
uniform mat4 MVP;

float PI = 3.14159265358979323846264;
float R = 6371;

vec3 geotocart( float lat, float lon, float radius )
{
	float PI = 3.14159265358979323846264;
	lat *= PI/180.0f;
	lon *= PI/180.0f;
	return radius*vec3(cos(lat) * cos(lon), cos(lat) * sin(lon), sin(lat));
}

void main(){ 
	float lat = (segment.x+1-position.y/1200);
	float lon = (segment.y+position.x/1200);

	vec3 gtc = geotocart(lat, lon, R);

	gl_Position.xyz = gtc + position.z/100*normalize(gtc);
	
    gl_Position.w = 1.0;

	gl_Position = MVP * gl_Position;

	ht = position.z;
}