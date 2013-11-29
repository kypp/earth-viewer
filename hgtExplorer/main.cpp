#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "shader.hpp"
#include "utils.h"
#include "segmentmanager.h"
#include <iostream>
#include <string>
#include <sstream>
#include <ostream>

using namespace glm;
using namespace std;

GLuint program, program2, program3;
GLuint vertexbuffer;
bool earth = true;
bool automat = false;

vec3 center = vec3(100,0,0);

segmentmanager Segments;

GLuint *indices;

GLuint parlocation, segparlocation, MatrixID, parlocation2, segparlocation2, MatrixID2, MatrixID3;
int LOD = 4;

//mat4 MVP;

double calcFPS(double theTimeInterval = 1.0, std::string theWindowTitle = "NONE");
vec3 geotocart(float lat, float lon, float radius);

void initialize()
{
	glfwInit();
	
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 16);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glewExperimental = GL_TRUE;
	
	glfwOpenWindow( 1920, 1080, 0,0,0,0, 32,0, GLFW_FULLSCREEN );
	
	glewInit();
	
	glfwSetWindowTitle( "HGTexplorer" );
	
	glfwEnable( GLFW_STICKY_KEYS );
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	program = LoadShaders( "VertexShaderHE.vert", "FragmentShaderHE.frag");
	program2 = LoadShaders( "VertexShader2HE.vert", "FragmentShaderHE.frag");
}

void movemap()
{
	static bool prevup = false;
	static bool prevdown = true;
	static bool mousemove = false;
	static bool prevaut = false;
	if (glfwGetKey('M')) earth = !earth;
	if (glfwGetKey(GLFW_KEY_UP)) center.x += 1;
	if (glfwGetKey(GLFW_KEY_DOWN)) center.x -= 1;
	if (glfwGetKey(GLFW_KEY_LEFT)) center.y -= 0.01;
	if (glfwGetKey(GLFW_KEY_RIGHT)) center.y += 0.01;
	if (glfwGetKey(GLFW_KEY_KP_ADD)) center.z += 0.01;
	if (glfwGetKey(GLFW_KEY_KP_SUBTRACT)) center.z -= 0.01;
	if (glfwGetKey('U') && (!prevaut)) 
	{ 
		automat = !automat;
		prevaut = true; 
	}
	else if (!glfwGetKey('U'))
		prevaut = false;
	if (glfwGetKey('=')) center.z *= 1.01;
	if (glfwGetKey('-')) center.z /= 1.01;
	if (glfwGetKey(']') && LOD < 128 && !prevup)  {LOD += 1;
	generateSurfaceOrder(1201, 1201, indices, LOD);}
	if (glfwGetKey('[') && LOD > 1 && !prevdown) {LOD -= 1;
	generateSurfaceOrder(1201, 1201, indices, LOD);}

	static vec3 movestart, mousestart; 
	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_1) && mousemove) {
		int x, y;
		glfwGetMousePos(&y, &x);
		center = movestart + vec3(1,-1,1)*(vec3(x,y,0)-mousestart)/1000.0f*1.0f/center.z;
		//center.x = movestart.x + (float)(x-movestart.x)/1000.0f;
		//center.y = movestart.y - (float)(y-movestart.y)/1000.0f;
		cout << x << endl << y << endl;
	}
	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_1) && !mousemove) {
		mousemove = true;
		movestart = center;
		int x, y;
		glfwGetMousePos(&y, &x);
		mousestart = vec3(x,y,0);
	}
	if (!glfwGetMouseButton(GLFW_MOUSE_BUTTON_1) && mousemove)
		mousemove = false;




	center.z = 1.0f+(float)glfwGetMouseWheel()/30.0f;

	prevup = glfwGetKey(']');
	prevdown = glfwGetKey('[');

	glUniform3f(earth? parlocation2:parlocation, center.x, center.y, center.z);
}


void flightkeymap()
{


}

void loop() 
{	
	static vec3 position = geotocart(49, 16, 6600);
	static vec3 angles = -normalize(position);//-normalize(position);
	
	static float velocity = 0.1f;
	static float pitch = 0, yaw = 0;


	if (glfwGetKey('Z'))
		velocity += 0.01;
	if (glfwGetKey('X'))
		velocity -= 0.01;

	float fact = 0.2f;
	
	if (glfwGetKey('A')) {		
		pitch+=fact;	
	}
	if (glfwGetKey('D')) {
		pitch-=fact;
	}
	if (glfwGetKey('W')) {
		yaw+=fact;
	}
	if (glfwGetKey('S')) {
		yaw-=fact;
	}

	mat4 rot = mat4(1.0f);
	rot = rotate(rot, pitch, vec3(0,1.0f,0));
	rot = rotate(rot, yaw, vec3(1.0f,0,0));
		
	vec3 pom = vec3(normalize(rot * vec4(angles, 0.0f)));
	vec3 direction = vec3(pom);

	vec3 up = vec3(0,1,0);
	vec3 right = up * direction;
	right = normalize(right);
	up = right * direction;


	position += velocity*normalize(direction);
	

	mat4 Projection = glm::perspective(60.0f, 16.0f/9.0f, 0.1f, 10000.f);

	mat4 View = lookAt(
	position,
	position+direction,
	up);

	
	mat4 Model      = glm::mat4(1.0f);  // Changes for each model !
	// Our ModelViewProjection : multiplication of our 3 matrices
	//Model = scale(Model, vec3(1, 1, 1));
	//Model = rotate(Model, x, vec3(1,0,0));


	mat4 MVP = Projection * View * Model;



	glUniformMatrix4fv(earth ? MatrixID2 : MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(MatrixID3, 1, GL_FALSE, &MVP[0][0]);

	movemap();

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//program = LoadShaders( "VertexShader.vert", "FragmentShader.frag");	
	/*if (earth) {

		glUseProgram(program3);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  
			3,                  // size
			GL_FLOAT,			// type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		glDrawArrays(GL_LINE_STRIP, 0, 72);
	}*/


	glUseProgram(earth? program2 : program);

	Segments.drawAll(LOD, indices, earth? segparlocation2 : segparlocation, center);	

	glfwSwapBuffers();
}

int main()
{
	initialize();

	Segments.loadrange(40, 60, 10, 50, "");

	center.x = 49;
	center.y = 16;
	center.z = 1;

	indices = new GLuint[1201*1201*3];

	generateSurfaceOrder(1201, 1201, indices, LOD);
	
	glUseProgramObjectARB(program);
  	parlocation = glGetUniformLocationARB(program, "par");
	segparlocation = glGetUniformLocationARB(program, "segment");
	MatrixID = glGetUniformLocation(program, "MVP");	

	glUseProgramObjectARB(program2);
	parlocation2 = glGetUniformLocationARB(program2, "par");
	segparlocation2 = glGetUniformLocationARB(program2, "segment");
	MatrixID2 = glGetUniformLocation(program2, "MVP");	

	glUseProgramObjectARB(program3);
	MatrixID3 = glGetUniformLocation(program3, "MVP");	

	GLuint VertexArrayID;

	do {
		loop();
		calcFPS(1.0, "HGTexplorer");
	} while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS && glfwGetWindowParam( GLFW_OPENED ) );


	glfwTerminate();

	delete [] indices;

	return 0;
}


double calcFPS(double theTimeInterval, std::string theWindowTitle)
{
	// Static values which only get initialised the first time the function runs
	static double t0Value       = glfwGetTime(); // Set the initial time to now
	static int    fpsFrameCount = 0;             // Set the initial FPS frame count to 0
	static double fps           = 0.0;           // Set the initial FPS value to 0.0

	// Get the current time in seconds since the program started (non-static, so executed every time)
	double currentTime = glfwGetTime();

	// Calculate and display the FPS every specified time interval
	if ((currentTime - t0Value) > theTimeInterval)
	{
		// Calculate the FPS as the number of frames divided by the interval in seconds
		fps = (double)fpsFrameCount / (currentTime - t0Value);

		// If the user specified a window title to append the FPS value to...
		if (theWindowTitle != "NONE")
		{
			// Convert the fps value into a string using an output stringstream
			std::ostringstream stream;
			stream << fps;
			std::string fpsString = stream.str();

			// Append the FPS value to the window title details
			theWindowTitle += " | FPS: " + fpsString;

			// Convert the new window title to a c_str and set it
			const char* pszConstString = theWindowTitle.c_str();
			glfwSetWindowTitle(pszConstString);
		}
		else // If the user didn't specify a window to append the FPS to then output the FPS to the console
		{
			std::cout << "FPS: " << fps << std::endl;
		}

		// Reset the FPS frame counter and set the initial time to be now
		fpsFrameCount = 0;
		t0Value = glfwGetTime();
		if (automat) 
		{
			if (fps<10){
				LOD++;
				generateSurfaceOrder(1201, 1201, indices, LOD); }
			if (fps>20 && LOD>1) {
				LOD--;
				generateSurfaceOrder(1201, 1201, indices, LOD); }
		}
	}
	else // FPS calculation time interval hasn't elapsed yet? Simply increment the FPS frame counter
	{
		fpsFrameCount++;
	}

	// Return the current FPS - doesn't have to be used if you don't want it!
	return fps;
}

glm::vec3 geotocart( float lat, float lon, float radius )
{
	float PI = 3.14159265358979323846264;
	lat *= PI/180.0f;
	lon *= PI/180.0f;
	//return radius*vec3(cos(lon) * cos(lat), cos(lon) * sin(lat), sin(lon));
	return radius*vec3(cos(lat) * cos(lon), cos(lat) * sin(lon), sin(lat));
}
