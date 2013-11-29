#version 330 core

layout(location=0) in vec3 position;
out float ht;
out vec3 Normal_cameraspace;
out vec3 LightDirection_cameraspace;
out vec2 UV;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform vec3 LightPosition_worldspace;

uniform vec3 par;
uniform vec2 segment; //x - szer, y - wys

const float PI = 3.14159265358979323846264;
const float R = 6371;

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

	//vec3 d = normalize(-gl_Position.xyz);

	vec4 pos = vec4(gtc + position.z/100*normalize(gtc) , 1);

	vec3 vertexNormal_modelspace = normalize(pos).xyz;

	gl_Position =  MVP * pos;
 
	vec3 Position_worldspace = (M * vec4(position,1)).xyz;
 
	vec3 vertexPosition_cameraspace = ( V * M * pos).xyz;
	vec3 EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;
 
 	vec3 LightPosition_cameraspace = ( V * vec4(LightPosition_worldspace,1)).xyz;
	LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
 
	Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace,0)).xyz;



	
	UV.x = 0.5 + lon/360.0f;	//lon
	UV.y = 0.5 + lat/180.0f;	//lat

	ht = position.z;


}