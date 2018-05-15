#pragma once
#include <GL/glew.h>

//SDL
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"


//glm
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp> // NOTE: must do before including GLM headers // NOTE: GLSL uses radians, so will do the same, for consistency
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

//other
#include <iostream>
#include <vector>
#include <sstream>

//assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Memory>
#include "shader.h"

class Load : public Shader
{
public:
	// CONSTRUCTOR
	Load();

	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 norm;
	};

	struct Mesh
	{
	public:
		std::vector<Vertex> Vertices;
		std::vector<GLuint> Indices;

		glm::vec3 scale;
		float angle = 0;

		Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices)
		{
			this->Vertices = vertices;
			this->Indices = indices;

			this->SetupMesh();

		}
		void DrawPlayer(glm::mat4 model, GLuint shaderProgram, float angle, float playerX, GLuint texture)
		{
			glUseProgram(shaderProgram);
			

			glBindVertexArray(this->VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
			glBindTexture(GL_TEXTURE_2D, texture);
			
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(playerX, -4.6f, -10.0f));
			model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));
			GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawElements(GL_TRIANGLES, (GLsizei)Indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		void DrawEnemy(glm::mat4 model, GLuint shaderProgram, float angle, float posx, float posy, float posz, GLuint texture)
		{
			glUseProgram(shaderProgram);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBindTexture(GL_TEXTURE_2D, texture);

			model = glm::mat4();
			model = glm::translate(model, glm::vec3(posx, posy, posz));
			model = glm::scale(model, glm::vec3(0.075f, 0.075f, 0.075f));
			model = glm::rotate(model, glm::radians(this->angle), glm::vec3(5.0f, 10.0f, 20.0f));
			
			GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawElements(GL_TRIANGLES, (GLsizei)Indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0); 
		}
		void DrawPBullet(glm::mat4 model, GLuint shaderProgram, float posx, float posy, GLuint texture)
		{
			glUseProgram(shaderProgram);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBindTexture(GL_TEXTURE_2D, texture);

			model = glm::mat4();
			model = glm::translate(model, glm::vec3(posx, posy, -10.0f));
			model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));

			GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawElements(GL_TRIANGLES, (GLsizei)Indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		void DrawEBullet(glm::mat4 model, GLuint shaderProgram, float posx, float posy, GLuint texture)
		{
			glUseProgram(shaderProgram);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBindTexture(GL_TEXTURE_2D, texture);

			model = glm::mat4();
			model = glm::translate(model, glm::vec3(posx, posy, -10.0f));
			model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

			GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawElements(GL_TRIANGLES, (GLsizei)Indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		void DrawBarrier(glm::mat4 model, GLuint shaderProgram, float posx, float posy, GLuint texture)
		{
			glUseProgram(shaderProgram);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBindTexture(GL_TEXTURE_2D, texture);

			model = glm::mat4();
			model = glm::translate(model, glm::vec3(posx, posy, -10.0f));
			model = glm::scale(model, glm::vec3(0.5f, 1.5f, 0.5f));

			GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawElements(GL_TRIANGLES, (GLsizei)Indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		void DrawBarricade(glm::mat4 model, GLuint shaderProgram, float posx, float posy, float posz, GLuint texture)
		{
			glUseProgram(shaderProgram);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBindTexture(GL_TEXTURE_2D, texture);

			model = glm::mat4();
			model = glm::translate(model, glm::vec3(posx, posy, posz));
			model = glm::scale(model, glm::vec3(0.15f, 0.3f, 0.15f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

			GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawElements(GL_TRIANGLES, (GLsizei)Indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		void DrawLives(glm::mat4 model, GLuint shaderProgram, float posx, float posy, float posz, GLuint texture)
		{
			glUseProgram(shaderProgram);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBindTexture(GL_TEXTURE_2D, texture);

			model = glm::mat4();
			model = glm::translate(model, glm::vec3(posx, posy, posz));
			model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawElements(GL_TRIANGLES, (GLsizei)Indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		void DrawSky(glm::mat4 model, GLuint shaderProgram, GLuint texture)
		{
			glUseProgram(shaderProgram);
			glBindVertexArray(this->VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
			glBindTexture(GL_TEXTURE_2D, texture);
;

			model = glm::mat4();
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(20.0f, 20.f, 20.0f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawElements(GL_TRIANGLES, (GLsizei)Indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

	private:
		GLuint VAO, VBO, EBO;

		void SetupMesh()
		{
			glGenVertexArrays(1, &this->VAO);
			glGenBuffers(1, &this->VBO);
			glGenBuffers(1, &this->EBO);

			glBindVertexArray(this->VAO);
			glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

			glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * this->Vertices.size(), &this->Vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * this->Indices.size(), &this->Indices[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, norm));

			glBindVertexArray(0);
		}
	};


	// FUNCTIONS
	void CreateWindow();
	void CreateOpenGL();
	void Initialise();
	void InitGL();
	void Render();
	void loadObj(std::string file);
	GLuint CreateTexture(char* path, SDL_Surface* surf);

	Shader shaderProgram, textShader, lightShader;
	float r = 0.5f;
	float g = 0.5f;
	float b = 0.5f;
	//float r = 1.0f;
	//float g = 1.0f;
	//float b = 1.0f;

	// WINDOW
	SDL_DisplayMode screenSize;
	int winW, winH;
	SDL_Window *win = nullptr;

	// OPENGL
	SDL_GLContext glcontext = nullptr;
	GLint success;
	GLuint VBO, EBO, VAO;
	GLuint tVBO, tEBO, tVAO;
	float aspectRatio;

	// GL TEXTURES
	GLuint tEnemy, tPlayer, tPlayerBullet, tBackground, tScore, tLives, tFrame, tShieldFull, tShieldMid, tShieldLast, tEnemyBullet, tExplosion;

	//PLAYER STUFF
	float playerX = 2.0f;
	float bulletX, bulletY;
	int counter = 0;

	// ENEMY STUFF
	bool enemyInit;
	glm::vec3 enemyPos[36];
	bool enemyAlive[36];
	glm::vec2 enemyBulletPos = {10.0f, 10.0f};
	float enemyAngle = 0;
	float bulletControl = 60.0f;

	// BARRIER STUFF
	glm::vec3 shield[3] = {{-3.5f ,-3.0f,-10.0f},{0.5f ,-3.0f,-10.0f},{4.5f,-3.0f,-10.0f}};
	float health[3] = {6.0f, 6.0f, 6.0f};

	//Explosion
	glm::vec2 explosionPos = { 10.0f, 10.0f };
	float eTimer;
	float eHide = 10.0f;

	// CONFUSING STUFF
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 orthoProj;
	GLint modelLocation, viewLocation, projectionLocation;
	glm::mat4 _transRotate;
	glm::mat4 _transTranslate;
	glm::mat4 _transScale;

	// TEXT STUFF
	SDL_Color textColor = { 1, 0, 0 };
	int scoreNum = 0;
	int livesNum = 3;
	char score[30] = {"Score: "};
	char lives[30] = {"Lives: "};
	float angle = 0.0f;
	SDL_Surface *scoreS, *livesS;
	GLfloat tVertices[20] = {
		// Positions          // Texture Coords
		0.0f,	0.2f,	0.0f,		0.0f, 1.0f,    // Top Left 
		0.13f,	0.2f,	0.0f,		1.0f, 1.0f,   // Top Right
		0.13f,	0.0f,	0.0f,		1.0f, 0.0f,   // Bottom Right
		0.0f,	0.0f,	0.0f,		0.0f, 0.0f   // Bottom Left
	};

	GLuint tIndices[6] = {
		0, 1, 2,  // First Triangle
		0, 2, 3,  // Second Triangle
	};

	//shader stuff
	GLuint vertex, fragment, text;

	//3d stuff
	std::vector <glm::vec3> vert;
	std::vector <glm::vec2> uv;
	std::vector <glm::vec3> norm;
	std::vector<std::unique_ptr<Mesh>> meshes;
	int cameraIndex = 0;
	float rotAngle = 0.0f;

	// DESTRUCTOR
	~Load();
private:
	
};