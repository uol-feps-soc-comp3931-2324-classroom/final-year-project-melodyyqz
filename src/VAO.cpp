#include"VAO.hpp"

// Constructor that generates a VAO ID
VAO::VAO()
{
	glGenVertexArrays(1, &ID);
}

// Links VBO to VAO using a certain layout
void VAO::LinkVBO(VBO& vbo, GLuint layout, GLuint numComponents, GLenum type, GLboolean normalized, GLsizei stride, void* offset) {
	vbo.Bind();
	glVertexAttribPointer(layout, numComponents, type, normalized, stride, offset);
	glEnableVertexAttribArray(layout);
	vbo.Unbind();
}

// Binds VAO
void VAO::Bind()
{
	glBindVertexArray(ID);
}

// Unbinds VAO
void VAO::Unbind()
{
	glBindVertexArray(0);
}

// Deletes VAO
void VAO::Delete()
{
	glDeleteVertexArrays(1, &ID);
}