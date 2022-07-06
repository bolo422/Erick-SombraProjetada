#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>

#include "shader.h"
#include "camera.h"

using namespace glm;
using namespace std;

const float MAX_LIGHT_X = 2;
const float MAX_LIGHT_Y = 3;
const float MAX_LIGHT_Z = 2;

enum TranslteAction
{
	horizontal,
	vertical,
	depth,
	origin,
	none
};

TranslteAction translteAction = origin;

float xlight, ylight, zlight;
float translateSpeed = 0.005;

Camera camera(vec3(-2.0f, 1.8f, 6.0f), vec3(0.0f, 1.0f, 0.0f), -70.0f, -20.0f);

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		translteAction = horizontal;

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		translteAction = vertical;

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		translteAction = depth;

	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		translteAction = origin;

	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		translteAction = none;

	if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		translateSpeed *= 1.1;
		clamp(translateSpeed, 1.0f, 0.0001f);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{
		translateSpeed *= 0.9;
		clamp(translateSpeed, 1.0f, 0.0001f);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_ENTER) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
	{
		translateSpeed = 0.005;
	}
	
	
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(1280, 720, "LearnOpenGL", nullptr, nullptr);
	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	if (window == nullptr) {
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		std::cout << "Failed no init GLEW." << std::endl;
		return EXIT_FAILURE;
	}

	glViewport(0, 0, screenWidth, screenHeight);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	Shader objectShader("Shaders/standard.vs", "Shaders/standard.fs", "Shaders/standard.gs");
	Shader groundShader("Shaders/ground.vs", "Shaders/ground.fs");
	Shader sunShader("Shaders/standard.vs", "Shaders/standard.fs", "Shaders/standard.gs");

	ifstream configFile("Textures/config.txt");
	string input;

	vector<float> cfg;

	while (configFile >> input)
	{
		if (input[0] == '#')
			continue;
		
		cfg.push_back(stof(input));
	}
	int size = cfg.size();
	int count = 0;

	vec3 lightColor = vec3(cfg[count++], cfg[count++], cfg[count++]);

	float objectPosition[] = {
		cfg[count++], cfg[count++], cfg[count++]
	};

	vec3 groundSize = vec3(cfg[count++], cfg[count++], cfg[count++]);
	float verticesGround[] = {
		// positions          // normals           // texture coords
		 groundSize.x,	groundSize.y,	 groundSize.z,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // top right
		 groundSize.x,	groundSize.y,	-groundSize.z,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-groundSize.x,	groundSize.y,	-groundSize.z,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, // bottom left
		-groundSize.x,	groundSize.y,	 groundSize.z,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	
	unsigned int indicesGround[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	float sunPosition[] = {
		cfg[count++], cfg[count++], cfg[count++]
	};

	configFile.close();

	unsigned int vboObject, vaoObject;
	glGenVertexArrays(1, &vaoObject);
	glGenBuffers(1, &vboObject);

	glBindVertexArray(vaoObject);
	glBindBuffer(GL_ARRAY_BUFFER, vboObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(objectPosition), objectPosition, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	unsigned int vboSun, vaoSun;
	glGenVertexArrays(1, &vaoSun);
	glGenBuffers(1, &vboSun);

	glBindVertexArray(vaoSun);
	glBindBuffer(GL_ARRAY_BUFFER, vboSun);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sunPosition), sunPosition, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	unsigned int vboGround, vaoGround, eboGround;
	glGenVertexArrays(1, &vaoGround);
	glGenBuffers(1, &vboGround);
	glGenBuffers(1, &eboGround);

	glBindVertexArray(vaoGround);
	glBindBuffer(GL_ARRAY_BUFFER, vboGround);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesGround), verticesGround, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboGround);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesGround), indicesGround, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	

	ifstream texturesFile("Textures/textures.txt");
	vector<string> textures;
	string commonpath;
	while (texturesFile >> input)
	{
		if (input[0] == '#')
			continue;

		if (commonpath.empty())
		{
			commonpath = input;
			continue;
		}

		if (commonpath == "n")
		{
			textures.push_back(input);
			continue;
		}

		textures.push_back(commonpath + input);
	}
	texturesFile.close();
	int countTex = 0;

	unsigned int textureGround, textureObject, sunTexture;
	// texture 1 - ground
	glGenTextures(1, &textureGround);
	glBindTexture(GL_TEXTURE_2D, textureGround);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	unsigned char* data = SOIL_load_image(textures[countTex++].c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	SOIL_free_image_data(data);

	// texture 2 - object
	glGenTextures(1, &textureObject);
	glBindTexture(GL_TEXTURE_2D, textureObject);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	data = SOIL_load_image(textures[countTex++].c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	SOIL_free_image_data(data);

	// texture 3
	glGenTextures(1, &sunTexture);
	glBindTexture(GL_TEXTURE_2D, sunTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	data = SOIL_load_image(textures[countTex++].c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	SOIL_free_image_data(data);

	objectShader.use();
	objectShader.setInt("tex", 0);

	sunShader.use();
	sunShader.setInt("tex", 0);

	groundShader.use();
	groundShader.setInt("groundTexture", 0);
	groundShader.setInt("objectTexture", 1);
	
	xlight = 0.0f;
	ylight = 1.9f;
	zlight = -0.2f;
	vec3 sunOrigin = vec3(xlight, ylight, zlight);

	bool right = false;
	bool down = false;
	bool back = false;

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);

		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render container
		mat4 view = camera.GetViewMatrix();
		mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)1280 / (float)720, 0.1f, 100.0f);
		mat4 model = glm::mat4(1.0f);

		switch (translteAction)
		{
		case horizontal:
			if (right)
			{
				xlight += translateSpeed;
				if (xlight >= MAX_LIGHT_X)
					right = false;
			}
			else
			{
				xlight -= translateSpeed;
				if (xlight <= -MAX_LIGHT_X)
					right = true;
			}
			break;
		case vertical:
			if (down)
			{
				ylight += translateSpeed;
				if (ylight >= MAX_LIGHT_Y)
					down = false;
			}
			else
			{
				ylight -= translateSpeed;
				if (ylight <= 0)
					down = true;
			}
			break;
		case depth:
			if (back)
			{
				zlight += translateSpeed;
				if (zlight >= MAX_LIGHT_Z)
					back = false;
			}
			else
			{
				zlight -= translateSpeed;
				if (zlight <= -MAX_LIGHT_Z)
					back = true;
			}
			break;
		case origin:
			xlight = sunOrigin.x;
			ylight = sunOrigin.y;
			zlight = sunOrigin.z;
			break;
		case none:
			break;
		}

		vec3 lightPos(xlight, ylight, zlight);

		//object
		objectShader.use();
		objectShader.setMat4("view", view);
		objectShader.setMat4("projection", projection);
		objectShader.setMat4("model", model);
		objectShader.setVec3("rect.topRight",		 2.5f, 5.0f, 0.0f);
		objectShader.setVec3("rect.topLeft",		-2.5f, 5.0f, 0.0f);
		objectShader.setVec3("rect.bottomRight",	2.5f, -5.0f, 0.0f);
		objectShader.setVec3("rect.bottomLeft", -2.5f, -5.0f, 0.0f);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureObject);
		glBindVertexArray(vaoObject);
		glDrawArrays(GL_POINTS, 0, 1);

		//ground
		groundShader.use();
		groundShader.setMat4("view", view);
		groundShader.setMat4("projection", projection);
		groundShader.setMat4("model", model);
		groundShader.setVec3("light.position", lightPos);
		groundShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		groundShader.setVec3("light.color", lightColor);
		groundShader.setVec3("cameraPos", camera.Position);

		groundShader.setVec3("rectTopRight",	10.6f, 10.6f, -10.5f);
		groundShader.setVec3("rectTopLeft",		-10.6f, 10.6f, -10.5f);
		groundShader.setVec3("rectBottomRight", 10.6f, -10.6f, -10.5f);
		groundShader.setVec3("rectBotomLeft",	-10.6f, -10.6f, -10.5f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureGround);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureObject);
		glBindVertexArray(vaoGround);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, sunTexture);

		sunShader.use();
		sunShader.setMat4("view", view);
		sunShader.setMat4("projection", projection);

		mat4 translate = glm::translate(glm::mat4(1.0f), lightPos);
		mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
		sunShader.setMat4("model", translate * scale);
		
		float scalefactor = 0.2f;
		sunShader.setVec3("rect.topRight", 2.5f * scalefactor, 5.0f * scalefactor, 0.0f);
		sunShader.setVec3("rect.topLeft", -2.5f * scalefactor, 5.0f * scalefactor, 0.0f);
		sunShader.setVec3("rect.bottomRight", 2.5f * scalefactor, -5.0f * scalefactor, 0.0f);
		sunShader.setVec3("rect.bottomLeft", -2.5f * scalefactor, -5.0f * scalefactor, 0.0f);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sunTexture);
		glBindVertexArray(vaoSun);
		glDrawArrays(GL_POINTS, 0, 1);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &vaoGround);
	glDeleteBuffers(1, &vboGround);
	glDeleteBuffers(1, &eboGround);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}