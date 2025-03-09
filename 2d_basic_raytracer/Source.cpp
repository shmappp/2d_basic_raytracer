#include <SDL.h>
#include <stdio.h>
#include <iostream>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define COLOUR_W 0xffffffff

int main(int argc, char* argv[]) {

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
	}
	
	SDL_Window* window = SDL_CreateWindow("rt", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	SDL_Surface* surface = SDL_GetWindowSurface(window);

	SDL_Rect rect = {200, 200, 200, 200};
	SDL_FillRect(surface, &rect, COLOUR_W);
	SDL_UpdateWindowSurface(window);

	SDL_Delay(3000);

	return 0;
	

}