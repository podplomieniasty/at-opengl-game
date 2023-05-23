#include <VBO.h>

VBO::VBO(std::vector<Vertex> vertices) {

	glGenBuffers(1, &this->id);
	glBindBuffer(GL_ARRAY_BUFFER, this->id);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

void VBO::bind() {

	glBindBuffer(GL_ARRAY_BUFFER, this->id);
}

void VBO::unbind() {

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::drop() {

	glDeleteBuffers(1, &this->id);
}