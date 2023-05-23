#include <Texture.h>

Texture::Texture(const char* image, const char* texType, GLuint slot) {

	type = texType;
	id = 0; // ¿eby usun¹æ warning
	unit = 0;

	int imgWidth, imgHeight, numColCh;

	stbi_set_flip_vertically_on_load(true); // obrazki wczytywa³y siê odbite, ta funkcja odbija
											// je z powrotem

	// u¿ycie bibilioteki stb do za³adowania obrazu
	unsigned char* bytes = stbi_load(image, &imgWidth, &imgHeight, &numColCh, 0);
	if (!bytes) {
		std::cout << "Failed to load texture file" << std::endl;
		return;
	}

	glGenTextures(1, &this->id);

	glActiveTexture(GL_TEXTURE0 + slot);
	this->unit = slot;

	glBindTexture(GL_TEXTURE_2D, this->id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (numColCh == 4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	else if (numColCh == 3) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
	else if (numColCh == 2) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		imgWidth, imgHeight, 0, GL_RED, GL_UNSIGNED_BYTE, bytes);
	else
		throw std::invalid_argument("TEXTURE: nie rozpoznano typu tekstury");

	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(bytes);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit) {

	GLuint texUni = glGetUniformLocation(shader.id, uniform);
	shader.activate();
	glUniform1i(texUni, unit);
}

void Texture::bind() {

	glActiveTexture(GL_TEXTURE0 + this->unit);
	glBindTexture(GL_TEXTURE_2D, this->id);
}

void Texture::unbind() {

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::drop() {

	glDeleteTextures(1, &this->id);
}