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

	1. Camera.h  -> wyœwietlanie obiektów i ich mapowanie
	2. Shader.h  -> konfiguracja i kompilowanie shaderów
	3. VAO.h     -> przechowuje tablicê obiektów vertex
	4. VBO.h     -> przechowuje vertice przekazywane do vertexów
	5. Texture.h -> konfiguracja i wyœwietlanie tekstur

	base.vert, base.frag -> domyœlny shader ³aduj¹cy obiekty


	Odnoszenie siê do modeli i zasobów:
		
		¯eby dostaæ siê do konkretnego zasobu u¿ywamy relatywnej
		œcie¿ki, np:
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
		std::cout << "B³¹d tworzenia okna aplikacji";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// inicjalizacja GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "B³¹d inicjalizacji GLAD";
		return -1;
	}

	// ---- DEKLARACJA P£ASZCZYZNY ----
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

	Camera camera(WIN_WIDTH, WIN_HEIGHT, glm::vec3(0.0f, 0.0f, 0.0f)); // kamera obejmuj¹ca ca³y ekran o pozycji startowej w punkcie vec3(0,0,0)
	Shader baseShaderProg("resources/shaders/base.vert", "resources/shaders/base.frag"); // domyœlny shader do obiektów

	/*
		Poniewa¿ aktualnie nie mamy dostêpu do Light shadera,
		kodujemy na "twardo" œwiat³o oœwietlaj¹ce model
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


	// pêtla renderuj¹ca
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