#include "DTEDsegment.h"
#include <fstream>
#include <iostream>
#include "utils.h"
using namespace std;




DTEDsegment::DTEDsegment(string filename, intpair coords, GLuint defaultVB, bool toGPU) : defaultVertexBuffer(defaultVB), coordinates(coords), IsOnGPU(false)
{
	ifstream filestream (filename, ios::in | ios::binary);
	FileLoaded = filestream;

	if (FileLoaded) {
		data = new float[SEGMENT_SIZE*SEGMENT_SIZE*3];
		char *buffer = new char[SEGMENT_SIZE*SEGMENT_SIZE*2];
		unsigned char *buf = (unsigned char *) buffer;
		filestream.read(buffer, SEGMENT_SIZE*SEGMENT_SIZE*2);
		int i=0, j=0;
		for (int y = 0; y < SEGMENT_SIZE*SEGMENT_SIZE; y++)
		{
			data[i++] = (float) (y%SEGMENT_SIZE);		//x coordinate
			data[i++] = (float) (y/SEGMENT_SIZE);		//y coordinate
			short h = (buf[2*y] << 8) + buf[2*y+1];
			data[i++] = (float) h>-500 && h<9000 || i<=3 ? h : data[i-3];	//height
		}	
		delete [] buffer;

		if (toGPU)
			sendToGPU();
		
	}


}


DTEDsegment::~DTEDsegment(void)
{	
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
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(data)*SEGMENT_SIZE*SEGMENT_SIZE*3, data, GL_STATIC_DRAW);
	
	IsOnGPU = true;
	delete [] data;
}

void DTEDsegment::Draw( int LOD, GLuint *indices, GLuint parlocation )
{
	glUniform2f(parlocation, coordinates.first, coordinates.second);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, FileLoaded?vertexbuffer:defaultVertexBuffer);
	glVertexAttribPointer(
		0,                  
		3,                  // size
		GL_FLOAT,			// type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);

	if (FileLoaded)
		glDrawElements(GL_TRIANGLE_STRIP, (SEGMENT_SIZE/LOD+SEGMENT_SIZE%LOD)*(SEGMENT_SIZE/LOD)*2, GL_UNSIGNED_INT, indices);
	else
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableVertexAttribArray(0);
}
