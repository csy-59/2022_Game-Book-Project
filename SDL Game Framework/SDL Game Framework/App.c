#include "stdafx.h"
#include "App.h"

#include "Framework/Window.h"
#include "Framework/Renderer.h"
#include "Framework/Image.h"
#include "Framework/Input.h"
#include "Framework/Timer.h"
#include "Framework/Random.h"
#include "Framework/Scene.h"

App g_App;

bool App_Init(void)
{
	if (0 != SDL_Init(SDL_INIT_VIDEO))
	{
		LogWithErrorCode("Fail to initialize SDL", SDL_GetError());

		return false;
	}

	Window_Init();
	if (NULL == g_App.Window)
	{
		LogWithErrorCode("Fail to intialize window", SDL_GetError());

		return false;
	}

	Renderer_Init();
	if (NULL == g_App.Renderer)
	{
		LogWithErrorCode("Fail to initialize renderer", SDL_GetError());

		return false;
	}

	if (false == Image_Init())
	{
		LogWithErrorCode("Fail to initalize image library", IMG_GetError());
	}

	Random_Init();

	return true;
}

void cleanup(void)
{
	Image_Cleanup();
	Renderer_Cleanup();
	Window_Cleanup();
	SDL_Quit();
}

void processInput(void)
{
	Input_Update();
}

void update(void)
{
	g_Scene.Update();
}

void render(void)
{
	g_Scene.Render();
	Renderer_Flip();
}

int32 App_Run(void)
{
	atexit(cleanup);

	Timer_Init(60);

	Scene_SetNextScene(SCENE_TITLE);

	SDL_Event event;
	while (true)
	{
		if (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				break;
			}
		}
		else
		{
			if (Scene_IsSetNextScene())
			{
				Scene_Change();
			}

			if (Timer_Update())
			{
				Renderer_Prepare();
				processInput();
				update();
				render();
			}
		}
	}


	return 0;
}