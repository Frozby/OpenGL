#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "stb_image.h"
#include "camera.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

unsigned int loadTexture(const char* path);
//         SETTINGS

// Screen-size
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 960;

// time setup
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// mouse setup
bool firstMouse = true;
const float mouse_sensitivity = 0.1f;
float lastX = SCR_WIDTH/2, lastY = SCR_HEIGHT/2;

// init camera object
Camera camera;


//define pointlight
struct pointLight {
	glm::vec3 pos;
	glm::vec3 color;
}typedef pointLight;

//sample colors in rgb
glm::vec3 colorRed = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 colorGreen = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 colorBlue = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 colorYellow = glm::vec3(1.0f, 1.0f, 0.0f);

// pointlights data
pointLight pointLights[] = {
	{glm::vec3(0.7f,  0.2f,  2.0f), colorRed},
	{glm::vec3(2.3f, -3.3f, -4.0f), colorBlue},
	{glm::vec3(-4.0f,  2.0f, -12.0f), colorGreen},
	{glm::vec3(0.0f,  0.0f, -3.0f), colorYellow}
};

GLuint pointLightCount = sizeof(pointLights)/sizeof(pointLights[0]);

// window size changed callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

	if (firstMouse) // initially set to true
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos; // reversed: y ranges bottom to top
	lastX = (float)xpos;
	lastY = (float)ypos;
	xoffset *= mouse_sensitivity;
	yoffset *= mouse_sensitivity;
	camera.ProcessMouseMovement(xoffset, yoffset, true);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll((float)yoffset);
}
void processInput(GLFWwindow* window)
{
	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	const float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera.ProcessKeyboard(DOWN, deltaTime);
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "openGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	gladLoadGL((GLADloadfunc)glfwGetProcAddress);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// capture mouse mode
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f

	};
	glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	Shader sourcelightshader("sourcelightvertex.shader", "sourcelightfragment.shader");
	Shader lightshader("lightvertex.shader", "lightfragment.shader");
	Shader shader("vertex.shader", "fragment.shader");

	glEnable(GL_DEPTH_TEST);
	unsigned int VAO, lightVAO, sourcelightVAO;
	glGenVertexArrays(1, &VAO);
	glGenVertexArrays(1, &lightVAO);
	glGenVertexArrays(1, &sourcelightVAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);

	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(sourcelightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	unsigned int diffuseMap = loadTexture("container.jpg");
	unsigned int specularMap = loadTexture("container_specular.jpg");

	//init matrices
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection;

	lightshader.use();
	lightshader.setInt("material.diffuse", 0);
	lightshader.setInt("material.specular", 1);

	//runtime render
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		view = camera.GetViewMatrix();


		//draw light Cubes
		sourcelightshader.use();
		sourcelightshader.setMat4("projection", projection);
		sourcelightshader.setMat4("view", view);


		for (GLuint i = 0; i < pointLightCount; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, pointLights[i].pos);

			sourcelightshader.setMat4("model",model);
			sourcelightshader.setVec3("color", pointLights[i].color.x, pointLights[i].color.y, pointLights[i].color.z);
			
			glBindVertexArray(sourcelightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		

		lightshader.use();
		lightshader.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
		
		lightshader.setMat4("projection", projection);
		lightshader.setMat4("view", view);


		// Direction light shader
		lightshader.setVec3("dirLight.direction", 0.3f,-1.0f,0.3f);
		lightshader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
		lightshader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f); // darkened
		lightshader.setVec3("dirLight.specular", 0.7f, 0.7f, 0.7f);
		


		// fill point lights array
		for (GLuint i = 0; i < pointLightCount; i++) {
			std::string pointlightname = "pointLights["+std::to_string(i)+"]";

			lightshader.setVec3(pointlightname +".position", pointLights[i].pos.x, pointLights[i].pos.y, pointLights[i].pos.z);

			lightshader.setVec3(pointlightname+".ambient", 0.0f, 0.0f, 0.0f);
			lightshader.setVec3(pointlightname+".diffuse", 0.7f*pointLights[i].color.x, 0.7f * pointLights[i].color.y, 0.7f * pointLights[i].color.z); // darkened
			lightshader.setVec3(pointlightname+".specular", 1.0f * pointLights[i].color.x, 1.0f * pointLights[i].color.y, 1.0f * pointLights[i].color.z);

			lightshader.setFloat(pointlightname+".constant", 1.0f);
			lightshader.setFloat(pointlightname+".linear", 0.045f);
			lightshader.setFloat(pointlightname+".quadratic", 0.0075f);
		
		}

		lightshader.setFloat("material.shininess", 64.0f);


		lightshader.setVec3("spotLight.position", camera.Position.x, camera.Position.y, camera.Position.z);
		lightshader.setVec3("spotLight.direction", camera.Front.x, camera.Front.y, camera.Front.z);

		lightshader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		lightshader.setVec3("spotLight.diffuse", 0.3f, 0.3f, 0.3f);
		lightshader.setVec3("spotLight.specular", 0.5f, 0.5f, 0.5f);

		lightshader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		lightshader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

		model = glm::mat4(1.0f);
		lightshader.setMat4("model", model);



		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		glBindVertexArray(lightVAO);
		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightshader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	
	}

	glfwDestroyWindow(window);
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &sourcelightVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;

}
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}