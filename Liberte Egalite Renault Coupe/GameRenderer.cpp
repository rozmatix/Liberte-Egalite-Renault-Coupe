#include "GameRenderer.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>


GameRenderer::GameRenderer()
	: window(nullptr), renderer(nullptr), spriteBatch(nullptr)
{
	// SDL_Init ni potreben v SDL3
}


GameRenderer::~GameRenderer()
{
	if (spriteBatch) delete spriteBatch;
	if (renderer) SDL_DestroyRenderer(renderer);
	if (window) SDL_DestroyWindow(window);
}


bool GameRenderer::Init(const char* title, int width, int height)
{
	window = SDL_CreateWindow(title, width, height, 0);
	if (!window)
	{
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return false;
	}


	renderer = SDL_CreateRenderer(window, nullptr);
	if (!renderer)
	{
		std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		return false;
	}


	spriteBatch = new SpriteBatch(renderer);
	return true;
}


void GameRenderer::Clear()
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	spriteBatch = new SpriteBatch(renderer);
}


void GameRenderer::Present()
{
	spriteBatch->Draw();
	SDL_RenderPresent(renderer);
}