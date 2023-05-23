#pragma once
#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <iostream>
#include <cerrno>

/*
	Shadery odpowiadaj� za spos�b renderowania i przedstawiania
	element�w na ekranie, mo�na nimi robi� �wiat�o, fale, zwyk�e obiekty
	itp.
	base        -> shader do wy�wietlania zwyk�ych obiekt�w 3d
	light (wip) -> o�wietlenie
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