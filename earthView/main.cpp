#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/virtrev/xstream.hpp>
#include <glm/gtx/transform.hpp>
#include "shader.hpp"
#include "texture.hpp"
#include "utils.h"
#include "segmentmanager.h"
#include <iostream>
#include <string>
#include <sstream>
#include <ostream>
#include "Sphere.cpp"

using namespace glm;
using namespace std;

GLuint program, program2, sphereprogram;
GLuint vertexbuffer;
bool earth = true;
bool automat = false;

vec3 center;

segmentmanager *Segments;

GLuint *indices;

GLuint parlocation, segparlocation, MatrixID, segparlocation2, MatrixID2, MatrixID3, TextureID, Texture, MatrixIDsphere, 
	SphereTextureID, TexGradID, ModelIDsphere, ViewIDsphere, LightIDsphere, NightTexture, NightTextureID, NightTextureID2, 
	ModelIDsphere2, ViewIDsphere2, LightIDsphere2;
int LOD = 4;
const int earthEastID = 0;

double calcFPS(double theTimeInterval = 1.0, std::string theWindowTitle = "NONE");
GLuint loadImage(const char* theFileName, GLenum minFilter, GLenum magFilter, GLenum wrapFilter);

void initialize()
{
	glfwInit();
	
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glewExperimental = GL_TRUE;
	
	
	glfwOpenWindow( 1920, 1080, 0,0,0,0, 32,0, GLFW_FULLSCREEN );
	
	glewInit();
	
	glfwSetWindowTitle( "HGTexplorer" );
	
	glfwEnable( GLFW_STICKY_KEYS );
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glDepthFunc(GL_LESS);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		
	program = LoadShaders( "VertexShaderEV.vert", "FragmentShaderEV.frag");
	program2 = LoadShaders( "VertexShader2EV.vert", "FragmentShaderEV.frag");
	sphereprogram = LoadShaders( "SphereShaderEV.vert", "FragmentShaderEV.frag");

	Texture = loadBMP_custom("images\\land_shallow_topo_8192.bmp");
	TextureID  = glGetUniformLocation(program2, "eastSampler");
	SphereTextureID  = glGetUniformLocation(sphereprogram, "eastSampler");

	NightTexture = loadBMP_custom("images\\land_ocean_lights_8192.bmp");
	NightTextureID = glGetUniformLocation(sphereprogram, "nightSampler");
	NightTextureID2 = glGetUniformLocation(program2, "nightSampler");
}

void movemap()
{
	static bool prevup = false;
	static bool prevdown = true;
	static bool mousemove = false;
	static bool prevaut = false;
	if (glfwGetKey('M')) earth = !earth;
	if (glfwGetKey(GLFW_KEY_UP)) center.x += 0.01;
	if (glfwGetKey(GLFW_KEY_DOWN)) center.x -= 0.01;
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

	glUniform3f(parlocation, center.x, center.y, center.z);
}


void loop() 
{		
	static float k = 0.0f;
	if (glfwGetKey('V')) 
		k+=1;
	if (glfwGetKey('B')) 
		k-=1;
	vec3 Light = geotocart(0,k,150000000);

	static vec3 position = geotocart(49, 16, 6600);
	static vec3 angles = -normalize(position);//-normalize(position);
	
	static float velocity = 1.0f;
	static float pitch = 0, yaw = 0;


	if (glfwGetKey('Z'))
		velocity += 50;
	if (glfwGetKey('X'))
		velocity -= 50;

	float fact = 0.2f;
	
	if (glfwGetKey('W')) {		
		pitch+=fact;	
	}
	if (glfwGetKey('S')) {
		pitch-=fact;
	}
	if (glfwGetKey('D')) {
		yaw+=fact;
	}
	if (glfwGetKey('A')) {
		yaw-=fact;
	}

	static float texgrad = 1.0f;

	if (glfwGetKey('T'))
		texgrad += 0.01f;
	if (glfwGetKey('G'))
		texgrad -= 0.01f;

	texgrad = texgrad>1.0f?1.0f:texgrad;
	texgrad = texgrad<0.0f?0.0f:texgrad;

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

	mat4 Projection = glm::perspective(60.0f, 16.0f/9.0f, 1.0f, 20000.f);

	position = geotocart(49+pitch, 16+yaw, 6600+velocity);
	mat4 View = lookAt(
	position,
	vec3(0),
	vec3(0,0,1)		
		);

	
	mat4 Model      = glm::mat4(1.0f);  

	mat4 MVP = Projection * View * Model;


	glUseProgram(earth? program2 : program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniform1i(TextureID, 0);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, NightTexture);
	glUniform1i(NightTextureID2, 1);

	glUniformMatrix4fv(earth ? MatrixID2 : MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelIDsphere2, 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(ViewIDsphere2, 1, GL_FALSE, &View[0][0]);
	glUniform3f(LightIDsphere2, Light.x, Light.y, Light.z);	
	glUniform1f(TexGradID, texgrad);

	movemap();

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	vec2 geoPos = carttogeo(normalize(position));
	int c = 20;
	

	Segments->drawAll(LOD, indices, earth? segparlocation2 : segparlocation, center, geoPos.y-c, geoPos.y+c, geoPos.x-c, geoPos.x+c);	

	static Sphere sphere(6371, 50, 50);
	
	static float angx = -270.0f;
	static float angy = 180.0f;
	static float angz = 180.0f;
	Model = scale(mat4(1.0f), vec3(-1.0f, -1.0f, -1.0f));
	Model = rotate(Model, angx, 1.0f, 0.0f, 0.0f);
	Model = rotate(Model, angy, 0.0f, 1.0f, 0.0f);
	Model = rotate(Model, angz, 0.0f, 0.0f, 1.0f);
	MVP = Projection * View * Model;


	glUseProgram(sphereprogram);
	glUniformMatrix4fv(MatrixIDsphere, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelIDsphere, 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(ViewIDsphere, 1, GL_FALSE, &View[0][0]);
	glUniform3f(LightIDsphere, Light.x, Light.y, Light.z);	

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniform1i(SphereTextureID, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, NightTexture);
	glUniform1i(NightTextureID, 1);

	sphere.draw(0,0,0);

	glfwSwapBuffers();
}

int main()
{
	initialize();

	Segments = new segmentmanager();

	Segments->loadrange(-90, 90, -180, 180, "images\\");

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
	segparlocation2 = glGetUniformLocationARB(program2, "segment");
	MatrixID2 = glGetUniformLocation(program2, "MVP");	
	TextureID  = glGetUniformLocation(program2, "eastSampler");
	TexGradID = glGetUniformLocation(program2, "texgrad");
	ModelIDsphere2 = glGetUniformLocation(program2, "M");
	ViewIDsphere2 = glGetUniformLocation(program2, "V");	
	LightIDsphere2 = glGetUniformLocation(program2, "LightPosition_worldspace");

	glUseProgramObjectARB(sphereprogram);
	MatrixIDsphere = glGetUniformLocation(sphereprogram, "MVP");
	ModelIDsphere = glGetUniformLocation(sphereprogram, "M");
	ViewIDsphere = glGetUniformLocation(sphereprogram, "V");	
	LightIDsphere = glGetUniformLocation(sphereprogram, "LightPosition_worldspace");

	GLuint VertexArrayID;

	do {
		loop();
		calcFPS(1.0, "HGTexplorer");
	} while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS && glfwGetWindowParam( GLFW_OPENED ) );


	glfwTerminate();

	delete [] indices;
	delete Segments;

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
