#include <string>
#include <exception>
#include <GL\glew.h>

#define SIZE 1201
typedef std::pair<int,int> intpair;

class DTEDsegment
{
public:
	DTEDsegment(std::string filename, intpair coords, bool toGPU = true);	//the constructor reads the file to the memory
	void sendToGPU(void);
	void Draw(int LOD, GLuint *indices, GLuint parlocation);
	~DTEDsegment(void);
	float *data;
	bool IsOnGPU;
	intpair coordinates;
	bool FileLoaded;
private:
	GLuint vertexbuffer;
	GLuint VertexArrayID;
};

