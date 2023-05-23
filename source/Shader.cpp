#include <Shader.h>

Shader::Shader(const char* vertexFile, const char* fragmentFile) {

	/*
		Shader odpowiada za sposób renderowania i przedstawiania
		obiektów graficznych. Tu siê go inicjalizuje
	*/

	// wczytanie zawartoœci plików
	std::string vertexCode   = getFileContents(vertexFile);
	std::string fragmentCode = getFileContents(fragmentFile);

	const char* vertexSrc    = vertexCode.c_str();
	const char* fragmentSrc  = fragmentCode.c_str();

	GLuint vertexShader   = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	std::cout << "SHADER: Kompilowanie [" << vertexFile << ", " << fragmentFile << "]\n";

	glShaderSource(vertexShader, 1, &vertexSrc, NULL);
	glCompileShader(vertexShader);
	checkForErrors(vertexShader, "VERTEX");

	glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
	glCompileShader(fragmentShader);
	checkForErrors(vertexShader, "FRAGMENT");

	// utworzenie programu i zalinkowanie go do shadera
	this->id = glCreateProgram();

	glAttachShader(this->id, fragmentShader);
	glAttachShader(this->id, vertexShader);

	glLinkProgram(this->id);
	checkForErrors(vertexShader, "PROGRAM");

	// po poprawnej kompilacji wyrzucamy to do œmieci
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	std::cout << "SHADER: Skonfigurowano shader: [" << vertexFile << ", " << fragmentFile << "]\n";
}

void Shader::activate() {

	glUseProgram(this->id);
}

void Shader::drop() {

	glDeleteProgram(this->id);
}

void Shader::checkForErrors(unsigned int shader, const char* type) {

	GLint hasCompiled;
	char log[1024];
	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE) {
			glGetShaderInfoLog(shader, 1024, NULL, log);
			std::cout << "SHADER_COMPILATION_ERROR dla typu:" << type << "\n" << std::endl;
		}
		return;
	}
	glGetProgramiv(shader, GL_COMPILE_STATUS, &hasCompiled);
	if (hasCompiled == GL_FALSE) {
		glGetProgramInfoLog(shader, 1024, NULL, log);
		std::cout << "SHADER_COMPILATION_ERROR dla typu:" << type << "\n" << std::endl;

	}
}

std::string getFileContents(const char* filename)
{
	// wczytanie zawartoœci pliku shadera
	std::ifstream input(filename, std::ios::binary);
	if (input) {
		std::string content;
		input.seekg(0, std::ios::end);
		content.resize(input.tellg());
		input.seekg(0, std::ios::beg);
		input.read(&content[0], content.size());
		input.close();
		std::cout << "SHADER: Zaladowano zasob: " << filename << "\n";
		return content;
	}
	std::cerr << "Nie znaleziono pliku!" << std::endl;
	throw(errno);
}
