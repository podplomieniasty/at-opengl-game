#pragma once
#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <iostream>
#include <cerrno>

/*
	Shadery odpowiadaj¹ za sposób renderowania i przedstawiania
	elementów na ekranie, mo¿na nimi robiæ œwiat³o, fale, zwyk³e obiekty
	itp.
	base        -> shader do wyœwietlania zwyk³ych obiektów 3d
	light (wip) -> oœwietlenie
	wave (wip?) -> fale wody
*/

class Shader {

	public:
		Shader(const char* vertexShader, const char* fragmentShader);

		GLuint id;
		void activate();
		void drop();
		void checkForErrors(unsigned int shader, const char* type);
};

std::string getFileContents(const char* filename);


#endif