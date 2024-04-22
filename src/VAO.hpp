#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include<glad/glad.h>
#include"VBO.hpp"

class VAO
{
public:
	GLuint ID;
	VAO();

	void LinkVBO(VBO& vbo, GLuint layout, GLuint numComponents, GLenum type, GLboolean normalized, GLsizei stride, void* offset);
	void Bind();
	void Unbind();
	void Delete();
};
#endif