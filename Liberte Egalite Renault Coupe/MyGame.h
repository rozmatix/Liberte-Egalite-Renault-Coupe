#ifndef MYGAME_H
#define MYGAME_H
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_Image.h>
#include <memory>
#include "Gameplay.h"


class MyGame {
public:
	bool Init();
	void Run();
	~MyGame();


private:
	SDL_Window* window = nullptr;
	std::unique_ptr<Gameplay> gameplay;
};


#endif

