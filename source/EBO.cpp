#include <EBO.h>

EBO::EBO(std::vector<GLuint>& indices) {

	glGenBuffers(1, &this->id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

void EBO::bind() {

	glBindBuffer(GL_ARRAY_BUFFER, this->id);
}

void EBO::unbind() {

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void EBO::drop() {

	glDeleteBuffers(1, &this->id);
}