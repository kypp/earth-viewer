#pragma once
#include <GL/glew.h>
#include <GL/glfw.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/virtrev/xstream.hpp>
#include <glm/gtx/transform.hpp>

#define M_PI 3.14159265358979323846264
#define M_PI_2 M_PI / 2


class Sphere
{
protected:
	std::vector<GLfloat> vertices;
	std::vector<GLfloat> normals;
	std::vector<GLfloat> texcoords;
	std::vector<GLushort> indices;

	GLuint vertexbuffer;
	GLuint VertexArrayID;
	GLuint texturebuffer;
	GLuint TextureArrayID;
	GLuint normalbuffer;
	GLuint NormalArrayID;

public:
	Sphere(float radius, unsigned int rings, unsigned int sectors)
	{
		float const R = 1./(float)(rings-1);
		float const S = 1./(float)(sectors-1);
		int r, s;

		vertices.resize(rings * sectors * 3);
		normals.resize(rings * sectors * 3);
		texcoords.resize(rings * sectors * 2);
		std::vector<GLfloat>::iterator v = vertices.begin();
		std::vector<GLfloat>::iterator n = normals.begin();
		std::vector<GLfloat>::iterator t = texcoords.begin();
		for(r = 0; r < rings; r++) 
		for(s = 0; s < sectors; s++) {
			float const y = sin( -M_PI_2 + M_PI * r * R );
			float const x = cos(2*M_PI * s * S) * sin( M_PI * r * R );
			float const z = sin(2*M_PI * s * S) * sin( M_PI * r * R );

			*t++ = s*S;
			*t++ = r*R;

			*v++ = x * radius;
			*v++ = y * radius;
			*v++ = z * radius;

			*n++ = x;
			*n++ = y;
			*n++ = z;
		}

		indices.resize(rings * sectors * 4);
		std::vector<GLushort>::iterator i = indices.begin();
		for(r = 0; r < rings-1; r++) for(s = 0; s < sectors-1; s++) {
			*i++ = r * sectors + s;
			*i++ = r * sectors + (s+1);
			*i++ = (r+1) * sectors + (s+1);
			*i++ = (r+1) * sectors + s;
		}

		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

		glBufferData(GL_ARRAY_BUFFER, sizeof(&vertices[0])*vertices.size(), &vertices[0], GL_STATIC_DRAW);

		glGenVertexArrays(1, &TextureArrayID);
		glBindVertexArray(TextureArrayID);

		glGenBuffers(1, &texturebuffer);
		glBindBuffer(GL_ARRAY_BUFFER, texturebuffer);

		glBufferData(GL_ARRAY_BUFFER, sizeof(&texcoords[0])*texcoords.size(), &texcoords[0], GL_STATIC_DRAW);

		glGenVertexArrays(1, &NormalArrayID);
		glBindVertexArray(NormalArrayID);

		glGenBuffers(1, &normalbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);

		glBufferData(GL_ARRAY_BUFFER, sizeof(&normals[0])*normals.size(), &normals[0], GL_STATIC_DRAW);

	}

	~Sphere()
	{
		glDeleteBuffers(1, &vertexbuffer);
		glDeleteVertexArrays(1, &VertexArrayID);
		glDeleteBuffers(1, &texturebuffer);
		glDeleteVertexArrays(1, &TextureArrayID);
		glDeleteBuffers(1, &normalbuffer);
		glDeleteVertexArrays(1, &NormalArrayID);
	}

	void draw(GLfloat x, GLfloat y, GLfloat z)
	{
		glm::translate(x, y, z);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, texturebuffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawElements(GL_TRIANGLE_STRIP_ADJACENCY, indices.size(), GL_UNSIGNED_SHORT, &indices[0]);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

	}
};