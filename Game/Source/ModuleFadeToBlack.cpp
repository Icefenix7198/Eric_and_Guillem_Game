#include "ModuleFadeToBlack.h"

#include "App.h"
#include "Render.h"
#include "Window.h"
#include "Log.h"
#include "Scene.h"

#include "SDL/include/SDL_render.h"

FadeToBlack::FadeToBlack() : Module()
{
	
}

FadeToBlack::~FadeToBlack()
{

}

bool FadeToBlack::Start()
{
	LOG("Preparing Fade Screen");//Quiza haya que cambiar el screen suferce con pixels to algo
	screenRect = { app->render->camera.x,app->render->camera.y,app->render->camera.w,app->render->camera.h }; //ERIC: Mirar si funciona bien el get scale
	// Enable blending mode for transparency
	//SDL_SetRenderDrawBlendMode(app->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

bool FadeToBlack::Update(float dt)
{
		// Exit this function if we are not performing a fade
		if (currentStep == Fade_Step::NONE) return true;

		if (currentStep == Fade_Step::TO_BLACK)
		{
			++frameCount;
			if (frameCount >= maxFadeFrames)
			{
			moduleToDisable->active=false;
			moduleToEnable->active=true;

			currentStep = Fade_Step::FROM_BLACK;
			}
		}
		else
		{
		--frameCount;
		if (frameCount <= 0)
		{
			currentStep = Fade_Step::NONE;
		}
		}

		return true;
}

bool FadeToBlack::PostUpdate()
{
	// Exit this function if we are not performing a fade
	if (currentStep == Fade_Step::NONE) return true;

	float fadeRatio = (float)frameCount / (float)maxFadeFrames;

	// Render the black square with alpha on the screen
	SDL_SetRenderDrawColor(app->render->renderer, 0, 0, 0, (Uint8)(fadeRatio * 255.0f));
	SDL_RenderFillRect(app->render->renderer, &screenRect);

	return true;
}

bool FadeToBlack::Change(Module* moduleToDisable, Module* moduleToEnable, float frames)
{
	bool ret = false;

	// If we are already in a fade process, ignore this call
	if(currentStep == Fade_Step::NONE)
	{
		currentStep = Fade_Step::TO_BLACK;
		frameCount = 0;
		maxFadeFrames = frames;

		this->moduleToDisable = moduleToDisable;
		this->moduleToEnable = moduleToEnable;

		ret = true;
	}

	return ret;
}

bool FadeToBlack::JustFade(float ms)
{

	for (int i = 0; i < 255; ++i)
	{
		app->render->DrawRectangle(screenRect, 255, 255, 255, i);
	}

	for (int i = 255; i > 0; --i)
	{
		app->render->DrawRectangle(screenRect, 255, 255, 255, i);
	}


	return true;
}