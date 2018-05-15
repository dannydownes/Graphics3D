#include "input.h"

Input::Input()
{
	fullscreen = false;
	enemyRight = true;
	enemyLeft = false;
	
}

void Input::Action(bool *run, Load &load)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			//registers keyboard events from the user
		case SDL_QUIT:
			SDL_Log("User closed application.");
			*run = false;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_F11:
				if (fullscreen == false)
				{
					fullscreen = true;
					SDL_SetWindowFullscreen(load.win, SDL_WINDOW_FULLSCREEN_DESKTOP);
					glViewport(0, 0, load.screenSize.w, load.screenSize.h);
					SDL_Log("User entered fullscreen . . .");
				}
				else if (fullscreen == true)
				{
					fullscreen = false;
					SDL_SetWindowFullscreen(load.win, SDL_WINDOW_SHOWN);
					SDL_SetWindowSize(load.win, load.winW, load.winH);
					glViewport(0, 0, load.winW, load.winH);
					SDL_Log("User exited fullscreen . . .");
				}
				break;
			case SDLK_ESCAPE:
				SDL_Log("User closed application . . .");
				*run = false;
				break;
			case SDLK_RIGHT:
			case SDLK_d:
				right = true;
				break;
			case SDLK_LEFT:
			case SDLK_a:
				left = true;
				break;
			case SDLK_SPACE:
				if (fire == false)
				{
					shotX = (load.playerX + (0.1055f / 2.0f));
				}
				fire = true;
				break;
			case SDLK_e:
				if (load.cameraIndex == 2)
				{
					load.cameraIndex = 0;
				}
				else
				{
					load.cameraIndex++;
				}
				break;
			default:
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			case SDLK_RIGHT:
			case SDLK_d:
				right = false;
				break;
			case SDLK_LEFT:
			case SDLK_a:
				left = false;
				break;
			default:
				break;
			}
		case SDL_MOUSEBUTTONDOWN:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
				if (fire == false)
				{
					shotX = (load.playerX);
				}
				fire = true;
				break;
			default:
				break;
			}
		default:
			break;
		}
	}
}

void Input::ActionUpdate(Load &load, bool &run)
{
	//ENEMY MOVEMENT
	for (int i = 0; i < 36; i++)
	{
		if (load.enemyPos[i].y < -4.0f && load.enemyAlive[i] == true)
		{
			run = false;
		}
		else if (load.enemyPos[i].x < -6.4f && load.enemyAlive[i] == true)
		{
			enemyRight = true;
			enemyLeft = false;
			for (int z = 0; z < 36; z++)
			{
				load.enemyPos[z].y -= enemyDrop;
			}
		}
		else if (load.enemyPos[i].x > 6.4f && load.enemyAlive[i] == true)
		{
			enemyLeft = true;
			enemyRight = false;

			for (int z = 0; z < 36; z++)
			{
				load.enemyPos[z].y -= enemyDrop;
			}
		}
	}
	for (int i = 0; i < 36; i++)
	{
		if (enemyRight == true && load.enemyAlive[i] == true)
		{
			load.enemyPos[i].x += enemySpeed;
		}
		if (enemyLeft == true && load.enemyAlive[i] == true)
		{
			load.enemyPos[i].x -= enemySpeed;
		}
	}

	// ENEMY BULLET
	load.enemyBulletPos.y -= 0.1f;

	if (load.bulletControl > 0)
	{
		load.bulletControl -= 1.0f;
	}

	for (int i = 0; i < 36; i++)
	{
		if (load.enemyPos[i].x >= (load.playerX - 0.2f) && load.enemyPos[i].x <= (load.playerX + 0.2f) && load.bulletControl == 0 && load.enemyAlive[i] == true)
		{
			load.enemyBulletPos.x = load.enemyPos[i].x + 0.025f;
			load.enemyBulletPos.y = load.enemyPos[i].y;
			load.bulletControl = 75.0f;
		}
	}

	// MOVE ENEMIES, SPIN OFF AND ENEMY BULLET COLLISIONS
	for (int i = 0; i < 36; i++)
	{
		if (((load.bulletX < load.enemyPos[i].x + 0.5f) && (load.bulletX >= load.enemyPos[i].x - 0.5f)) && ((load.bulletY < load.enemyPos[i].y + 0.5f) && (load.bulletY > load.enemyPos[i].y - 0.5f)) && load.enemyAlive[i] == true)
		{
			load.enemyAlive[i] = false;
			allDead--;
			printf("enemy killed\n");
			load.scoreNum += 5;
			printf("Score: %d \n", load.scoreNum);
			if (allDead % 6 == 0)
			{
				enemySpeed = enemySpeed * 1.8f;
			}
			fire = false;
		}
		if (enemyRight == true && load.enemyAlive[i] == true)
		{
			load.enemyPos[i].x += enemySpeed;
		}
		else if (enemyLeft == true && load.enemyAlive[i] == true)
		{
			load.enemyPos[i].x -= enemySpeed;
		}
		if (load.enemyAlive[i] == false)
		{
			load.meshes[i + 1]->angle += 15;
			load.enemyPos[i].y += 0.05f;
			load.enemyPos[i].z -= 0.3f;
		}
	}

	// BULLET BARRIER COLLISION
	for (int i = 0; i < 3; i++)
	{
		if (load.health[i] <= 0)
		{
			//load.shield[i].y = 10.0f;
			load.shield[i].z -= 0.3f;
			load.shield[i].y += 0.05f;
		}
		if ((load.bulletX < load.shield[i].x + 0.5f) && (load.bulletX >= load.shield[i].x - 1.7f) && (load.bulletY < load.shield[i].y + 0.5f) && (load.bulletY > load.shield[i].y - 0.5f))
		{
			load.health[i]--;
			fire = false;
		}
		if ((load.enemyBulletPos.x < load.shield[i].x + 0.5f) && (load.enemyBulletPos.x >= load.shield[i].x - 1.7f) && (load.enemyBulletPos.y < load.shield[i].y + 0.5f) && (load.enemyBulletPos.y > load.shield[i].y - 0.5f))
		{
			load.health[i]--;
			load.enemyBulletPos.x = 10.0f;
		}
		for (int j = 0; j < 36; j++)
		{
			if ((((load.enemyPos[j].x < load.shield[i].x + 0.5f) && (load.enemyPos[j].x < load.shield[i].x - 1.7f)) || ((load.enemyPos[j].x + 0.2 < load.shield[i].x + 0.5f) && (load.enemyPos[j].x + 0.2 < load.shield[i].x))) && ((load.enemyPos[j].y < load.shield[i].y + 0.3)))
			{
				load.health[i] -= 0.2f;
			}
		}
	}
	if (load.enemyBulletPos.x < load.playerX + 0.5f && load.enemyBulletPos.x > load.playerX - 0.5f && load.enemyBulletPos.y < -4.75f)
	{
		load.livesNum -= 1;
		load.enemyBulletPos.x = 10.0f;
		printf("player hit\n");
	}

	// ENDS GAME IF ENEMIES DEAD
	if (allDead == 0)
	{
		printf("Winner\n");
		run = false;
	}
	if (load.livesNum == 0)
	{
		printf("Loser\n");
		run = false;
	}

	// PLAYER BARRIER COLLISIONS
	if (left == true)
	{
		if (load.playerX <= -6.1f)
		{
			left = false;
		}
		else
		{
			load.playerX -= 0.1f;
		}
	}

	if (right == true)
	{
		if (load.playerX >= 6.1f)
		{
			right = false;
		}
		else
		{
			load.playerX += 0.1f;
		}
	}

	//// SHOOTING
	if (fire == false)
	{
		load.bulletX = load.playerX;
		load.bulletY = -5.0f;
	}
	if (fire == true)
	{
		load.bulletX = shotX;
		if (load.bulletY < 10.0f)
		{
			load.bulletY += 0.5f;
		}
		else
		{
			fire = false;
		}
	}
	SDL_itoa(load.scoreNum, sNum, 10);
	for (int i = 7; i < 10; i++)
	{
		load.score[i] = sNum[i - 7];
	}
	//load.Text(load.score, load.tScore, load.scoreS);

	SDL_itoa(load.livesNum, lNum, 10);
	for (int i = 7; i < 10; i++)
	{
		load.lives[i] = lNum[i - 7];
	}
	//load.Text(load.lives, load.tLives, load.livesS);
} 