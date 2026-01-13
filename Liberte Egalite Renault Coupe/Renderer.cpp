#include "Renderer.h"
#include <SDL3/SDL.h>
#include <vector>


Renderer::Renderer() : window(nullptr), renderer(nullptr) {}


Renderer::~Renderer() {
	Shutdown();
}


bool Renderer::Init(const char* title, int width, int height) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) return false;


	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height, SDL_WINDOW_SHOWN);
	if (!window) return false;


	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) return false;


	return true;
}


void Renderer::Begin() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
}


void Renderer::DrawSprite(const Sprite& s) {
	SDL_Rect dst;
	dst.x = (int)s.x;
	dst.y = (int)s.y;
	dst.w = (int)s.w;
	dst.h = (int)s.h;


	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &dst);
}


void Renderer::End() {
	SDL_RenderPresent(renderer);
}


void Renderer::Shutdown() {
	if (renderer) SDL_DestroyRenderer(renderer);
	if (window) SDL_DestroyWindow(window);
	SDL_Quit();
}
