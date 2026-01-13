#include "MyGame.h"
#include <iostream>
#include <SDL3_image/SDL_Image.h>

bool MyGame::Init() {
	SDL_Init(SDL_INIT_VIDEO);



	window = SDL_CreateWindow("SDL3 Sprite Renderer", 100, 100, 1024, 768);
	if (!window) return false;


	gameplay = std::make_unique<Gameplay>(window);
	return gameplay->LoadAssets();
}


void MyGame::Run() { gameplay->Run(); }


MyGame::~MyGame() {
	SDL_DestroyWindow(window);
	SDL_Quit();
}
