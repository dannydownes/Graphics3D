#include "load.h"

Load::Load()
{
	Initialise();
	SDL_GetDesktopDisplayMode(0, &screenSize);
	aspectRatio = 4 / 3;
	winW = screenSize.w / 2;
	winH = screenSize.h / 2;
	CreateWindow();
	CreateOpenGL();
	InitGL();

	SDL_Surface sPlayer;
	tPlayer = CreateTexture("..//assets//grey.png", &sPlayer);
	SDL_Surface walls;
	tFrame = CreateTexture("..//assets//Sky_Blue.png", &walls);
	SDL_Surface sBullet;
	tEnemyBullet = CreateTexture("..//assets//Red.png", &sBullet);
	SDL_Surface sBarrier;
	tBackground = CreateTexture("..//assets//Green.png", &sBarrier);

	this->shaderProgram = Shader("..//src//vert.shader", "..//src//frag.shader");
	this->textShader = Shader("..//src//textVert.shader", "..//src//textFrag.shader");
	this->lightShader = Shader("..//src//lightVert.shader", "..//src//lightFrag.shader");
}

void Load::Initialise()
{
	// SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init Error: %s\n", SDL_GetError());
	}
	SDL_Log("SDL initialised OK!\n");

	// SDL_Image
	int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_image init error: %s\n", IMG_GetError());
	}
	SDL_Log("SDL_image initialised OK!\n");

	// SDL_ttf
	if (TTF_Init() == -1)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_ttf init error: %s\n", SDL_GetError());
	}
	SDL_Log("SDL_ttf initialised OK!\n");

	// SDL_mixer
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_mixer init error: %s\n   PERHAPS NO HEADPHONES/SPEAKERS CONNECTED\n", Mix_GetError());
	}
	SDL_Log("SDL_mixer initialised OK!\n");
}

void Load::CreateWindow()
{
	win = SDL_CreateWindow("Daniel Downes | Graphics CGP2012M | 14520077", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winW, winH, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (win == nullptr)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow init error: %s\n", SDL_GetError());
	}
}

void Load::CreateOpenGL()
{
	// Set OpenGL context parameters
	int major = 4, minor = 3;
	SDL_Log("Asking for OpenGL %d.%d context\n", major, minor);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_PROFILE_CORE); //use core profile
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); //ask for forward compatible
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); //default, probably

	glcontext = SDL_GL_CreateContext(win); // OpenGL Context Creation
	if (glcontext == NULL)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_CreateContext init error: %s\n", SDL_GetError());
	}
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
	SDL_Log("Got an OpenGL %d.%d context\n", major, minor);

	GLenum rev; // initialise GLEW - sets up the OpenGL function pointers for the version of OpenGL we're using
	glewExperimental = GL_TRUE; //https://www.opengl.org/wiki/OpenGL_Loading_Library#GLEW
	rev = glewInit();
	if (GLEW_OK != rev)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "glewInit error: %s\n", glewGetErrorString(rev));
	}
	SDL_Log("glew initialised OK!\n");

	glViewport(0, 0, winW, winH);
}

void Load::InitGL()
{
	glUseProgram(this->shaderProgram.Use());
	//player object
	//position 0
	loadObj("..//assets//playerShip.obj");
	
	//enemy object
	//position 1-37
	for (int i = 0; i < 36; i++)
	{
		loadObj("..//assets//Invader.obj");
	}
	for (float y = 1.5f; y < 5.5f; y++)
	{
		for (float x = -6.0f; x < 3.0f; x++)
		{
			enemyAlive[counter] = true;
			enemyPos[counter] = { x, y, -10.0};
			counter++;
		}
	}

	//bullets
	//pos 38-39
	loadObj("..//assets//PlayerBullet.obj");
	loadObj("..//assets//PlayerBullet.obj");

	//barrier
	//pos 40 41
	loadObj("..//assets//barrier.obj");
	loadObj("..//assets//barrier.obj");

	//shield
	//pos 42,43,44
	loadObj("..//assets//barrier.obj");
	loadObj("..//assets//barrier.obj");
	loadObj("..//assets//barrier.obj");

	//LIVES
	loadObj("..//assets//lives.obj");
	loadObj("..//assets//lives.obj");
	loadObj("..//assets//lives.obj");

	loadObj("..//assets//skybox.obj");

	//orthographic projection
	//first is where its looking, second is where camera is, third
	//view = glm::lookAt(glm::vec3(playerX + 6, 3, 2), glm::vec3(playerX + 6, 4, -1), glm::vec3(0, 1, 0));
	//view = glm::scale(view, glm::vec3(4, 4, 1));
	//left, right, bottom, top, near clip plane, far clip plane
	//projection = glm::ortho(0.0f, 4.0f, 0.0f, 3.0f, -1.0f, 100.0f);
	//gluLookAt();
	projection = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);
	projectionLocation = glGetUniformLocation(this->shaderProgram.Use(), "projection");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
}

void Load::Render()
{
	// LIGHTING
	GLint lightLocation = glGetUniformLocation(this->shaderProgram.Use(), "lightColour");
	glUniform3f(lightLocation, r, g, b);
	GLfloat lightDist = 0.0f;
	GLint lightDistLocation = glGetUniformLocation(this->shaderProgram.Use(), "lightDistance");
	glUniform1f(lightDistLocation, 1.0 + lightDist);
	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// CAMERA POSITION SWITCH
	if (cameraIndex == 0)
	{
		view = glm::lookAt(glm::vec3{ 0.0f, 0.0f, 3.0 }, glm::vec3{ 0.0f, 0.0f, 0.0 }, glm::vec3{ 0.0, 1.0, 0.0 });
	}
	if (cameraIndex == 1)
	{
		view = glm::lookAt(glm::vec3{ 15.0f, 20.0f, 10.0 }, glm::vec3{ -5.0f, 0.0f, -10.0 }, glm::vec3{ 0.0, 1.0, 0.0 });
	}
	if (cameraIndex == 2)
	{
		view = glm::lookAt(glm::vec3{ playerX, -4.0f, -10.0 }, glm::vec3{ playerX, 100, -10.0 }, glm::vec3{ 0.0, 0.0, 1.0 });
	}

	glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram.Use(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram.Use(), "view"), 1, GL_FALSE, glm::value_ptr(view));

	// PLAYER
	meshes[0]->DrawPlayer(model, this->shaderProgram.Use(), rotAngle, playerX, tPlayer);
	
	//ENEMIES
	for (int i = 1; i < 37; i++)
	{
		meshes[i]->DrawEnemy(model, this->shaderProgram.Use(), enemyAngle, enemyPos[i-1].x, enemyPos[i-1].y, enemyPos[i-1].z, tPlayer);
	}

	//BULLETS
	meshes[37]->DrawPBullet(model, this->shaderProgram.Use(), bulletX , bulletY, tEnemyBullet);
	meshes[38]->DrawEBullet(model, this->shaderProgram.Use(), enemyBulletPos.x, enemyBulletPos.y, tEnemyBullet);

	//BARRIER
	meshes[39]->DrawBarrier(model, this->shaderProgram.Use(), -7.5f, -7.0f, tEnemyBullet);
	meshes[40]->DrawBarrier(model, this->shaderProgram.Use(), 7.5f, -7.0f, tEnemyBullet);

	//SHIELDS
	for (int i = 0; i < 3; i++)
	{
		meshes[41 + i]->DrawBarricade(model, this->shaderProgram.Use(), shield[i].x, shield[i].y, shield[i].z, tBackground);
	}
	
	if (livesNum >= 3)
	{
		meshes[44]->DrawLives(model, this->shaderProgram.Use(), -6.0f, 4.5f, -10.0f, tEnemyBullet);
		meshes[45]->DrawLives(model, this->shaderProgram.Use(), -5.0f, 4.5f, -10.0f, tEnemyBullet);
		meshes[46]->DrawLives(model, this->shaderProgram.Use(), -4.0f, 4.5f, -10.0f, tEnemyBullet);
	}
	else if (livesNum >= 2 && livesNum < 3)
	{
		meshes[44]->DrawLives(model, this->shaderProgram.Use(), -6.0f, 4.5f, -10.0f, tEnemyBullet);
		meshes[45]->DrawLives(model, this->shaderProgram.Use(), -5.0f, 4.5f, -10.0f, tEnemyBullet);
	}
	else if (livesNum >= 1 && livesNum < 2)
	{
		meshes[44]->DrawLives(model, this->shaderProgram.Use(), -6.0f, 4.5f, -10.0f, tEnemyBullet);
	}																			

	meshes[47]->DrawSky(model, this->shaderProgram.Use(), tFrame);

	SDL_GL_SwapWindow(win);
}

GLuint Load::CreateTexture(char* path, SDL_Surface* surf)
{
	GLuint tex;
	surf = IMG_Load(path);
	if (surf == NULL)
	{
		std::cout << "Image load failed...." << std::endl;
	}

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	//texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	SDL_FreeSurface(surf);

	return tex;
}

void Load::loadObj(std::string file)
{
	// IMPORTS FILE
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_GenNormals);

	// ERROR CHECKING
	if ((!scene) || (scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE) || (!scene->mRootNode))
	{
		std::cerr << "Error loading mymodel.obj: " << std::string(importer.GetErrorString()) << std::endl;
	}

	//MOVES DATA TO TEMP VERTEX
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[i];

		std::vector<Vertex> Vertices;
		std::vector<GLuint> Indices;

		for (GLuint j = 0; j < mesh->mNumVertices; ++j)
		{
			Vertex tempVertex;

			tempVertex.pos.x = mesh->mVertices[j].x;
			tempVertex.pos.y = mesh->mVertices[j].y;
			tempVertex.pos.z = mesh->mVertices[j].z;

			tempVertex.norm.x = mesh->mNormals[j].x;
			tempVertex.norm.y = mesh->mNormals[j].y;
			tempVertex.norm.z = mesh->mNormals[j].z;

			Vertices.push_back(tempVertex);
		}

		for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
		{
			aiFace face = mesh->mFaces[j];
			for (unsigned int k = 0; k < face.mNumIndices; ++k) { Indices.push_back(face.mIndices[k]); }
		}
		//PUSHES BACK ONTO VECTOR FOR USE ELSEWHERE
		this->meshes.push_back(std::unique_ptr<Mesh>(new Mesh(std::move(Vertices), std::move(Indices))));
	}
}

Load::~Load()
{
	SDL_Log("Finished. Cleaning up and closing down\n");

	//Mix_FreeChunk(scratch);
	//Mix_FreeMusic(music);
	//Mix_CloseAudio();
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	SDL_GL_DeleteContext(glcontext);
	SDL_Quit();
}