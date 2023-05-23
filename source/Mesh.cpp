#include <Mesh.h>

Mesh::Mesh(std::vector <Vertex>& vertices,
	std::vector <GLuint>& indices,
	std::vector <Texture>& textures) {

	Mesh::vertices = vertices;
	Mesh::indices = indices;
	Mesh::textures = textures;

	// fancy!!!
	Mesh::vao.bind();


	VBO vbo(vertices); // klasa przechowujaca vertice
	EBO ebo(indices);   // klasa przechowujaca indices

	// pozycja attribow jest wazna!
	// link attrib pozycja
	vao.linkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	// normalna
	vao.linkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	// kolory
	vao.linkAttrib(vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	// tekstura
	vao.linkAttrib(vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

	Mesh::vao.unbind();
	vbo.unbind();
	ebo.unbind();
}

void Mesh::draw(
	Shader& shader,
	Camera& camera,
	glm::mat4 matrix,
	glm::vec3 translation,
	glm::quat rotation,
	glm::vec3 scale
) {

	shader.activate();
	Mesh::vao.bind();

	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;
	for (unsigned int i = 0; i < Mesh::textures.size(); i++) {

		std::string num;
		std::string type = Mesh::textures[i].type;
		if (type == "diffuse") {
			num = std::to_string(numDiffuse++);
		}
		else if (type == "specular") {
			num = std::to_string(numSpecular++);
		}
		Mesh::textures[i].texUnit(shader, (type + num).c_str(), i);
		Mesh::textures[i].bind();

	}

	glUniform3f(glGetUniformLocation(shader.id, "camPos"), camera.position.x, camera.position.y, camera.position.z);
	camera.matrix(shader, "camMatrix");

	glm::mat4 trans = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 sca = glm::mat4(1.0f);

	trans = glm::translate(trans, translation);
	rot = glm::mat4_cast(rotation);
	sca = glm::scale(sca, scale);

	glUniformMatrix4fv(glGetUniformLocation(shader.id, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
	glUniformMatrix4fv(glGetUniformLocation(shader.id, "rotation"), 1, GL_FALSE, glm::value_ptr(rot));
	glUniformMatrix4fv(glGetUniformLocation(shader.id, "scale"), 1, GL_FALSE, glm::value_ptr(sca));
	glUniformMatrix4fv(glGetUniformLocation(shader.id, "model"), 1, GL_FALSE, glm::value_ptr(matrix));



	glDrawElements(GL_TRIANGLES, Mesh::indices.size(), GL_UNSIGNED_INT, 0);
}