#pragma once

// OpenGL dependencies
#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include "stb_image.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Standard library dependecies
#include <iostream>
#include <vector>
#include <unordered_set>

// User defined dependencies
#include "Shader.h"
#include "Camera.h"

class QuadRenderer
{
public:
	GLuint VAO;
	GLuint VBO;
	GLuint texture;
	Shader shader;
	Camera& camera;
	std::vector<glm::vec3> cubePositions;
	std::unordered_set<int> xPositions;
	std::unordered_set<int> zPositions;

	float vertices[180] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	QuadRenderer(int SCR_WIDTH, int SCR_HEIGHT, Camera &camera) : shader("vertexShader.vert", "fragmentShader.frag"), camera(camera)
	{
		createCubeTexture();
		cubePositions = generateCubePositions();

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shader.setMat4("projection", projection);

		// Create a VAO to store information about the vertices
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// Bind the vertex data to the buffer so it can be drawn
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Configure the position parameters
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Configure the texture parameters
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	void render()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		shader.use();

		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);

		generateWorldProcedurally();

		for (glm::vec3 cube : cubePositions)
		{
			glm::mat4 model(1.0f);
			model = glm::translate(model, cube);
			//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.3f, 0.5f, 0.8f));
			shader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}	
	}

	void createCubeTexture()
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);

		unsigned char *data = stbi_load("img/block_texture/dirt.jpg", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		} else
		{
			std::cout << "Failed to load texture" << std::endl;
		}

		stbi_image_free(data);

		shader.use();
		shader.setInt("texture", 0);
	}

	std::vector<glm::vec3> generateCubePositions()
	{
		std::vector<glm::vec3> cubePositions;

		for (int z = 0; z < 16; z++)
		{
			for (int x = 0; x < 16; x++)
			{
				cubePositions.push_back(glm::vec3(camera.Position.x - x, -2, camera.Position.z - z));
			}
		}

		return cubePositions;
	}

	void addCubeCoordinates()
	{
		for (int i = -15; i < 15; i++)
		{
			for (int j = -15; j < 15; j++)
			{
				xPositions.insert(camera.Position.x + j);
				zPositions.insert(camera.Position.z + i);
			}
		}
	}

	void generateWorldProcedurally()
	{
		cubePositions.clear();
		addCubeCoordinates();

		for (int xpos : xPositions)
		{
			for (int zpos : zPositions)
			{
				cubePositions.push_back(glm::vec3(xpos, -2, zpos));
			}
		}
		
		// Interesting to look at: std::cout << "Number of cubes: " << cubePositions.size() << std::endl;
	}
};