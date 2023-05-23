#include <VAO.h>

VAO::VAO() {

	glGenVertexArrays(1, &this->id);
}

void VAO::linkAttrib(VBO& vbo, GLuint layout,
	GLuint numComponents, GLenum type,
	GLsizeiptr stride, void* offset) {

	vbo.bind();
	glVertexAttribPointer(layout, numComponents,
		type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	vbo.unbind();
}

void VAO::bind() {

	glBindVertexArray(this->id);
}

void VAO::unbind() {

	glBindVertexArray(0);
}

void VAO::drop() {

	glDeleteVertexArrays(1, &this->id);
}