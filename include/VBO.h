#pragma once
#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>

struct Vertex {

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 textUV;
};

class VBO {
public:
	GLuint id;
		VBO(std::vector<Vertex> vertices);

		void bind();
		void unbind();
		void drop();
};

#endif