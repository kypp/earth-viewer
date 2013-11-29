#version 330 core

in float ht;
out vec3 color;

void main(){
	if      (ht < 0  )   color = vec3(0.,       0.,        0.); //black
    else if (ht < 500)   color = vec3(0.,       ht/500,    0.); //->green
    else if (ht < 1000)  color = vec3(ht/500-1, 1.,        0.); //->yellow
    else if (ht < 1500)  color = vec3(1.,       1-(ht-1000)/500, 0.); //->red
    else if (ht < 9000)	 color = vec3(1.,		(ht-1500)/500,			(ht-1500)/500);                //white
	else color = vec3(0,0,0);



	//color = vec3(ht/1000, ht/1000, ht/1000);
}