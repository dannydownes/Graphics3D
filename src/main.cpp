#include <GL/glew.h> // GLEW - OpenGL Extension Wrangler - http://glew.sourceforge.net/ // NOTE: include before SDL.h

#include "SDL.h" // SDL - Simple DirectMedia Layer - https://www.libsdl.org/
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

#define GLM_FORCE_RADIANS // OpenGL Mathematics - https://glm.g-truc.net/ // force glm to use radians 
#include <glm/glm.hpp> // NOTE: must do before including GLM headers // NOTE: GLSL uses radians, so will do the same, for consistency
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "load.h"
#include "input.h"
#include "shader.h"
#include <iostream>
#include <atltime.h>

int main(int argc, char *argv[]) 
{
	bool run = true;
	Load load;
	Input input;

	const double MSPU = 1000 / 60;
	double prev = GetCurrentTime();
	double lag = 0.0;

	while (run)
	{	
		double current = GetCurrentTime();
		double elapsed = current - prev;
		prev = current;
		lag += elapsed;

		// INPUT
		input.Action(&run, load);
		// INPUT

		while (lag >= MSPU)
		{
			// UPDATE
			input.ActionUpdate(load, run);
			lag -= MSPU;
			// UPDATE
		}

		// RENDER
		load.Render();
		// RENDER
	}
	
	//// Load the music
	//Mix_Music *music = Mix_LoadMUS("assets/beat.wav");
	//if (!music) {
	//  SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Mix_LoadMUS error: %s\n",
	//              Mix_GetError());
	//  return 1;
	//}
	//SDL_Log("SDL_mixer loaded music OK!\n");

	//// Load the sound effect
	//Mix_Chunk *scratch = Mix_LoadWAV("assets/scratch.wav");
	//if (!scratch) {
	//  SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Mix_LoadWAV error: %s\n",
	//              Mix_GetError());
	 //  return 1;
	 //}
	 //SDL_Log("SDL_mixer loaded sound effect OK!\n");
	 //// Play the music and sound effect
	 //Mix_PlayChannel(-1, scratch, 0);
	 //Mix_PlayMusic(music, -1);

	// Clean up
	load.~Load();
	return 0;
}
