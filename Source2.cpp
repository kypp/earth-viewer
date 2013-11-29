#define GL3_PROTOTYPES
#include <GL3/gl3.h>
#include <SDL.h>
#include <iostream>
#include <fstream>
#include <stack>

//OpenGL Mathematics (GLM).  A C++ mathematics library for 3D graphics.
#include <glm/glm.hpp>
#include <glm/GLM_VIRTREV_address.h> // gives helper function address(X) instead of using &amp;X[0][0]
#include <glm/GLM_GTX_transform.h>
#include <glm/GLM_GTX_matrix_projection.h>
#include <glm/GLM_GTX_transform2.h> // for lookAt
using namespace glm;

#include <IL/ilut.h> //http://openil.sourceforge.net/tuts/tut_step/index.htm

struct TextureHandle {
	ILubyte *p;  /* pointer to image data loaded into memory */
	ILuint id;   /* unique DevIL id of image */
	ILint w;     /* image width */
	ILint h;     /* image height */
	ILenum DestFormat;             /* see ilConvertImage */
	ILenum DestType;               /* see ilConvertImage */
};
TextureHandle frac;

//---+----3----+----2----+----1----+---><---+----1----+----2----+----3----+----4
//----------------------------   LoadImageDevIL   ------------------------------

// use devIL, cross platform image loading library(openil.sourceforge.net/about.php)

ILuint LoadImageDevIL (char *szFileName, struct TextureHandle *T)
{
	//When IL_ORIGIN_SET enabled, the origin is specified at an absolute 
	//position, and all images loaded or saved adhere to this set origin.
	ilEnable(IL_ORIGIN_SET);
	//sets the origin to be IL_ORIGIN_LOWER_LEFT when loading all images, so 
	//that any image with a different origin will be flipped to have the set 
	//origin.
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	//Now load the image file
	ILuint ImageNameID;
	ilGenImages(1, &amp;ImageNameID);
	ilBindImage(ImageNameID);
	if (!ilLoadImage(szFileName)) return 0; // failure 
	ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE); // set destination image properties

	T->id = ImageNameID;
	T->p = ilGetData(); 
	T->w = ilGetInteger(IL_IMAGE_WIDTH);
	T->h = ilGetInteger(IL_IMAGE_HEIGHT);
	T->DestFormat = ilGetInteger(IL_IMAGE_FORMAT);
	T->DestType = ilGetInteger(IL_IMAGE_TYPE);

	printf("%s %d %d %d\n",szFileName,T->id,T->w,T->h);
	return 1; // success
}


//---+----3----+----2----+----1----+---<>---+----1----+----2----+----3----+----4
//---------------------------   cleanup_images   -------------------------------

//  Clear out the memory used by loading image files.

void cleanup_images(void) {
	printf("cleanup image memory:{");

	if (frac.id) {
		printf(" %d",frac.id); 
		ilDeleteImages(1, &amp;frac.id);
	} 

	printf(" }\n");
}


void glError(const char *file, int line) {
	GLenum err(glGetError());

	while(err!=GL_NO_ERROR) {
		std::string error;

		switch(err) {
		case GL_INVALID_OPERATION:  error="INVALID_OPERATION";  break;
		case GL_INVALID_ENUM:    error="INVALID_ENUM";    break;
		case GL_INVALID_VALUE:    error="INVALID_VALUE";    break;
		case GL_OUT_OF_MEMORY:    error="OUT_OF_MEMORY";    break;
		}

		std::cerr<<"GL_"<<error<<" - "<<file<<':'<<line<<std::endl;
		err=glGetError();
	}
}

int main() {
	//SDL Initialization
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,2);
	SDL_Init(SDL_INIT_VIDEO);

	const int r_width(640),r_height(480);
	SDL_WindowID window(SDL_CreateWindow("Foo",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,r_width,r_height,SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN));
	SDL_GLContext glcontext(SDL_GL_CreateContext(window));

	std::cout<<"OpenGL "<<glGetString(GL_VERSION)<<" (GLSL "<<glGetString(GL_SHADING_LANGUAGE_VERSION)<<')'<<std::endl;

	// Read in textures from disk (bind to openGL next though ...)
	ilInit();
	assert( LoadImageDevIL ((char*)"Frac.png", &amp;frac) );
	atexit(cleanup_images);

	//Bind texture to opengl
	GLuint textureid;

	glGenTextures(1,&amp;textureid);
	glBindTexture(GL_TEXTURE_2D,textureid);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexImage2D (GL_TEXTURE_2D, 0, frac.DestFormat, frac.w, frac.h, 0, frac.DestFormat, frac.DestType, frac.p);
	glGenerateMipmap(GL_TEXTURE_2D); //Generate mipmaps now!!!

	//Create shaders and shader program
	GLuint vshader(glCreateShader(GL_VERTEX_SHADER));
	GLuint fshader(glCreateShader(GL_FRAGMENT_SHADER));
	GLuint program(glCreateProgram());

	{
		std::ifstream source_file("image.vert");

		std::string data;
		std::getline(source_file,data,'\0');

		const GLchar *vshader_source(data.c_str());

		glShaderSource(vshader,1,&amp;vshader_source,NULL);
	}

	{
		std::ifstream source_file("image.frag");

		std::string data;
		std::getline(source_file,data,'\0');

		const GLchar *fshader_source(data.c_str());

		glShaderSource(fshader,1,&amp;fshader_source,NULL);
	}

	glCompileShader(vshader);
	glCompileShader(fshader);

	glAttachShader(program,vshader);
	glAttachShader(program,fshader);
	glLinkProgram(program);

	//Get handles to shader uniforms
	GLint matrixloc(glGetUniformLocation(program,"projmat")); //uniform mat4 projmat;
	GLint matrix2loc(glGetUniformLocation(program,"modelmat")); //uniform mat4 modelmat;
	GLint texloc(glGetUniformLocation(program,"tex")); //uniform sampler2D tex;

	//Create geometry vertex array
	GLuint tris,coords,vao;
	const GLfloat vertices[3*2]= {0.0f,0.0f,  0.0f,1.0f,   1.0f,1.0f};
	const GLfloat texcoords[3*2]={0.0f,0.0f,  0.0f,1.0f,   1.0f,1.0f};

	glGenVertexArrays(1,&amp;vao);
	glBindVertexArray(vao);

	//in vec2 vert;
	glGenBuffers(1,&amp;tris);
	glBindBuffer(GL_ARRAY_BUFFER,tris);
	glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
	const GLint location(glGetAttribLocation(program,"vert"));
	glVertexAttribPointer(location,2,GL_FLOAT,GL_TRUE,0,NULL);
	glEnableVertexAttribArray(location);

	//in vec2 texcoord;
	glGenBuffers(1,&amp;coords);
	glBindBuffer(GL_ARRAY_BUFFER,coords);
	glBufferData(GL_ARRAY_BUFFER,sizeof(texcoords),texcoords,GL_STATIC_DRAW);
	const GLint coordloc(glGetAttribLocation(program,"texcoord"));
	glVertexAttribPointer(coordloc,2,GL_FLOAT,GL_TRUE,0,NULL);
	glEnableVertexAttribArray(coordloc);

	//Render
	glm::mat4 projmat;
	glm::mat4 modelmat;

	projmat=glm::mat4(1.0);  //loadIdentity
	projmat*=glm::perspective(45.0f, float(r_width/r_height), 0.1f, 100.0f);

	modelmat=glm::mat4(1.0);  //loadIdentity
	modelmat*=glm::lookAt( glm::vec3(0.5f,0.5f,0.5f), 
		glm::vec3(0.5f,0.5f,0.0f), 
		glm::vec3(0.0f,1.0f,0.0f));
	modelmat*=glm::translate(glm::vec3(0.0,0.0,-1.0f));

	glUseProgram(program);

	glUniformMatrix4fv(matrixloc,1,GL_FALSE,&amp;projmat[0][0]);
	glUniformMatrix4fv(matrix2loc,1,GL_FALSE,&amp;modelmat[0][0]);

	glUniform1i(texloc,0); // 0 for GL_TEXTURE0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,textureid);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES,0,3);

	glError(__FILE__,__LINE__);
	SDL_GL_SwapWindow(window);
	SDL_Delay(6000);

	//Quit
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	glDeleteProgram(program);
	glDeleteTextures(1,&amp;textureid);
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();
}