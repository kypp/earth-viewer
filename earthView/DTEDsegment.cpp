#include "DTEDsegment.h"
#include <fstream>
#include <iostream>
#include "utils.h"
using namespace std;


DTEDsegment::DTEDsegment(string filename, intpair coords, bool toGPU)
{
	IsOnGPU = false;
	ifstream filestream (filename, ios::in | ios::binary);
	FileLoaded = filestream;
	if (!FileLoaded) return;

	data = new float[SIZE*SIZE*3];
	char *buffer = new char[SIZE*SIZE*2];
	unsigned char *buf = (unsigned char *) buffer;
	filestream.read(buffer, SIZE*SIZE*2);
	int i=0;
	for (int y = 0; y < SIZE*SIZE; y++)
	{
		data[i++] = (float) (y%SIZE);		//x coordinate
		data[i++] = (float) (y/SIZE);		//y coordinate
		short h = (buf[2*y] << 8) + buf[2*y+1];
		data[i++] = (float) h>-500 && h<9000 || i<=3 ? h : data[i-3];	//height
	}	
	delete [] buffer;

	coordinates = coords;

	if (toGPU)
		sendToGPU();
}


DTEDsegment::~DTEDsegment(void)
{	
	if (FileLoaded)
		delete [] data;
	if (IsOnGPU) {
		glDeleteBuffers(1, &vertexbuffer);
		glDeleteVertexArrays(1, &VertexArrayID);
	}
}

void DTEDsegment::sendToGPU( void )
{
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data)*SIZE*SIZE*3, data, GL_STATIC_DRAW);

	IsOnGPU = true;
}

void DTEDsegment::Draw( int LOD, GLuint *indices, GLuint parlocation )
{
	glUniform2f(parlocation, coordinates.first, coordinates.second);

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

	//glDrawArrays(GL_POINTS, 0, 1201*1200*2);
	//glDrawElements(GL_TRIANGLE_STRIP, 1201*1200*2, GL_UNSIGNED_INT, indices);
	glDrawElements(GL_TRIANGLE_STRIP, (SIZE/LOD+SIZE%LOD)*(SIZE/LOD)*2, GL_UNSIGNED_INT, indices);

	glDisableVertexAttribArray(0);
}
