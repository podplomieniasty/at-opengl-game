#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Shader.h>
#include <Camera.h>
#include <VBO.h>
#include <Texture.h>
#include <Mesh.h>

#include <assimp/mesh.h>
#include <Model.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 800
#define WIN_TITLE "Bialas, Burzec i Kijowski - Elektryczne Boogaloo 2"

/*
	CO ZA CO ODPOWIADA CO GDZIE DOKUMENTACJA

	1. Camera.h  -> wy�wietlanie obiekt�w i ich mapowanie
	2. Shader.h  -> konfiguracja i kompilowanie shader�w
	3. VAO.h     -> przechowuje tablic� obiekt�w vertex
	4. VBO.h     -> przechowuje vertice przekazywane do vertex�w
	5. Texture.h -> konfiguracja i wy�wietlanie tekstur

	base.vert, base.frag -> domy�lny shader �aduj�cy obiekty


	Odnoszenie si� do modeli i zasob�w:
		
		�eby dosta� si� do konkretnego zasobu u�ywamy relatywnej
		�cie�ki, np:
			base.vert     = resources/shaders/base.vert;
			swiatynia.obj = resources/models/swiatynia.obj
*/

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

	glViewport(0, 0, width, height);
}

int main() {

	// ---- INICJALIZACJA GLFW, GLAD ---- //


	// inicjalizacja GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// tworzenie okna
	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, NULL, NULL);
	if (window == NULL)
	{
		std::cout << "B��d tworzenia okna aplikacji";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// inicjalizacja GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "B��d inicjalizacji GLAD";
		return -1;
	}

	// ---- DEKLARACJA P�ASZCZYZNY ----
	Vertex vertices[] =
	{ //               COORDINATES           /            COLORS          /           NORMALS         /       TEXTURE COORDINATES    //
		Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
		Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
		Vertex{glm::vec3(1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
		Vertex{glm::vec3(1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
	};

	GLuint indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	Texture textures[] = { Texture("resources/textures/obama.png", "diffuse", 0) };

	std::vector<Vertex> verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector<GLuint> ind(indices, indices + sizeof(indices) / sizeof(GLuint));
	std::vector<Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));

	Mesh floor(verts, ind, tex);

	Model sword("resources/models/upo kolumna/untitled.gltf");

	// ---- OBIEKTY GRY ---- //

	Camera camera(WIN_WIDTH, WIN_HEIGHT, glm::vec3(0.0f, 0.0f, 0.0f)); // kamera obejmuj�ca ca�y ekran o pozycji startowej w punkcie vec3(0,0,0)
	Shader baseShaderProg("resources/shaders/base.vert", "resources/shaders/base.frag"); // domy�lny shader do obiekt�w

	/*
		Poniewa� aktualnie nie mamy dost�pu do Light shadera,
		kodujemy na "twardo" �wiat�o o�wietlaj�ce model
	*/
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);

	lightModel = glm::translate(lightModel, lightPos);

	// w celu przekazania wlasciwosci do shaderow, uzywamy uniformow
	baseShaderProg.activate();
	glUniform4f(glGetUniformLocation(baseShaderProg.id, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(baseShaderProg.id, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	glEnable(GL_DEPTH_TEST);


	// p�tla renderuj�ca
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.inputs(window);
		camera.updateMatrix(45.0f, 0.1f, 100.0f);
		floor.draw(baseShaderProg, camera);
		sword.draw(baseShaderProg, camera);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	baseShaderProg.drop();
	glfwTerminate();
	return 0;
}