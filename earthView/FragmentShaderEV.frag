#version 330 core

in float ht;
in vec2 UV;
in vec3 Normal_cameraspace;
in vec3 LightDirection_cameraspace;

out vec3 color;

uniform sampler2D eastSampler;
uniform sampler2D nightSampler;
uniform float texgrad;

void main(){

	vec3 gcol = vec3(0,0,0);

	if (ht < 0);
	else if (ht < 500)   gcol = vec3(0.,       ht/500,    0.); //->green
    else if (ht < 1000)  gcol = vec3(ht/500-1, 1.,        0.); //->yellow
    else if (ht < 1500)  gcol = vec3(1.,       1-(ht-1000)/500, 0.); //->red
    else if (ht < 9000)	 gcol = vec3(1.,		(ht-1500)/500,			(ht-1500)/500);                //white
	
	// Normal of the computed fragment, in camera space
	vec3 n = normalize( Normal_cameraspace );
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize( LightDirection_cameraspace );
	float cosTheta = clamp( dot( n,l ), 0,1 );
 
	color = texture2D(eastSampler, UV).rgb * cosTheta;	/*+ vec3(0.1,0.1,0.1)*/

	if (cosTheta <= 0.2f) {
		vec3 c = texture2D(nightSampler, UV).gbr;
		color = color + (0.2-cosTheta)*c;
		}

	gcol *= cosTheta;

	if (length(gcol) >= 0.01f)
		color = (texgrad*color + (1.0f-texgrad)*gcol);
}