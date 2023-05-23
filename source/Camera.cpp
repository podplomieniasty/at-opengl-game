#include <Camera.h>

Camera::Camera(int width, int height, glm::vec3 position) {

	// inicjzalizacja pozycji startowej

	this->width = width;
	this->height = height;
	this->position = position;
}

void Camera::updateMatrix(float fov, float nearPlane, float farPlane) {


	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);

	view = glm::lookAt(this->position, this->position + this->orientation, this->up);
	proj = glm::perspective(glm::radians(fov), (float)(this->width / this->height), nearPlane, farPlane);

	this->cameraMatrix = proj * view; // KOLEJNOŒC MA ZNACZENIE
}

void Camera::matrix(Shader& shader, const char* uniform) {

	glUniformMatrix4fv(glGetUniformLocation(shader.id, uniform), 1, GL_FALSE, glm::value_ptr(this->cameraMatrix));
}

void Camera::inputs(GLFWwindow* window) {

	// przechwytuje akcje z okienka window
	// i mapuje je do odpowiednich czynnosci

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		this->position += this->speed * this->orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		this->position += this->speed * -glm::normalize(glm::cross(this->orientation, this->up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		this->position += this->speed * -(this->orientation);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		this->position += this->speed * glm::normalize(glm::cross(this->orientation, this->up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		this->position += this->speed * this->up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		this->position += this->speed * -(this->up);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		this->speed = 0.4f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
		speed = 0.1f;
	}

	// sterowanie myszk¹
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		if (this->firstClick) {
			glfwSetCursorPos(window, this->width / 2, this->height / 2);
			this->firstClick = false;
		}

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		float rotx = this->sensitivity * (float)(mouseY - (this->height / 2)) / this->height;
		float roty = this->sensitivity * (float)(mouseX - (this->height / 2)) / this->height;

		glm::vec3 newOrient = glm::rotate(this->orientation, glm::radians(-rotx), glm::normalize(glm::cross(this->orientation, this->up)));

		if (!(glm::angle(newOrient, this->up) <= glm::radians(5.0f)
			|| glm::angle(newOrient, -this->up) <= glm::radians(5.0f))) {

			this->orientation = newOrient;
		}

		this->orientation = glm::rotate(this->orientation, glm::radians(-roty), this->up);

		glfwSetCursorPos(window, this->width / 2, this->height / 2);
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		this->firstClick = true;
	}

}