#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include <math.h>

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900
#define COLOUR_W 0xffffffff
#define COLOUR_BL 0x00000000

struct Circle {
	double x;
	double y;
	double radius;
};

void drawRect(SDL_Surface* surface, SDL_Rect* rect) {
	SDL_FillRect(surface, rect, COLOUR_W);
}

void putPixel(SDL_Surface* surface, int x, int y, Uint32 colour) {
	if (x < 0 || x >= surface->w || y < 0 || y >= surface->h) { return; }
	Uint32* pixels = (Uint32*)surface->pixels;
	pixels[(y * surface->w) + x] = colour;
}

void drawCircle(SDL_Surface* surface, Circle circle) {
	SDL_LockSurface(surface);
	for (double x = circle.x - circle.radius; x <= circle.x + circle.radius; x++) {
		for (double y = circle.y - circle.radius; y <= circle.y + circle.radius; y++) {
			double dist = sqrt(pow(x - circle.x, 2) + pow(y - circle.y, 2));
			if (dist <= circle.radius) {
				putPixel(surface, x, y, COLOUR_W);
			}

		}
	}
	SDL_UnlockSurface(surface);
}

int main(int argc, char* argv[]) {

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
	}
	
	SDL_Window* window = SDL_CreateWindow("rt", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	SDL_Surface* surface = SDL_GetWindowSurface(window);

	SDL_Rect blankScreen = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
	Circle circle = { 500, 200, 200 };
	//SDL_Rect rect = {200, 200, 200, 200};
	//SDL_FillRect(surface, &rect, COLOUR_W);
	//SDL_UpdateWindowSurface(window);

	SDL_Event event;
	int running = 1;

	while (running) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					running = 0;
					break;

				case SDL_MOUSEMOTION:
					circle.x = event.motion.x;
					circle.y = event.motion.y;

			}
		}
		SDL_FillRect(surface, &blankScreen, COLOUR_BL);
		drawCircle(surface, circle);
		SDL_UpdateWindowSurface(window);
	}

	SDL_Delay(3000);

	return 0;
	

}