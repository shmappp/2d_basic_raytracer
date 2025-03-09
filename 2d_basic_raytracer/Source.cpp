#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <vector>


#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900
#define COLOUR_W 0xffffffff
#define COLOUR_BL 0x00000000
#define NUM_RAYS 200

class Shape {
public:
	virtual ~Shape() {}
	virtual bool intersects(double inc_x, double inc_y) const = 0;
};

class Circle : public Shape {
public:
	double x, y, radius;

	Circle(double x, double y, double radius) : x(x), y(y), radius(radius) {}

	bool intersects(double inc_x, double inc_y) const override {
		return (
			(inc_x >= x - radius && inc_x <= x + radius) &&
			(inc_y >= y - radius && inc_y <= y + radius) && 
			sqrt(pow(inc_x - x, 2) + pow(inc_y - y, 2)) <= radius
			) ? true : false;
	}
};

//struct Circle {
//	double x;
//	double y;
//	double radius;
//};

struct Ray {
	double x_origin, y_origin;
	double angle;
};

std::ostream& operator<<(std::ostream& os, const Ray& ray) {
	os << "Ray: x = " << ray.x_origin << ", y = " << ray.y_origin << ", angle (radians) = " << ray.angle << ", angle (deg) = " << ray.angle * 180/M_PI;
	return os;
}

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

std::vector<Ray> generateRaySet(Circle circle) {
	std::vector<Ray> rays;

	double angleSlice = 2 * M_PI / NUM_RAYS; // in radians

	for (int i = 0; i < NUM_RAYS; i++) {
		Ray ray = {circle.x, circle.y, angleSlice*(i+1)};
		rays.push_back(ray);
	}

	return rays;
}

void drawRays(SDL_Surface* surface, Circle circle) {
	std::vector<Ray> rays = generateRaySet(circle);
	for (Ray ray : rays) {
		double curr_x = ray.x_origin;
		double curr_y = ray.y_origin;
		while (curr_x >= 0 && curr_x <= WINDOW_WIDTH && curr_y >= 0 && curr_y <= WINDOW_HEIGHT) {
			curr_x += cos(ray.angle);
			curr_y += sin(ray.angle);
			putPixel(surface, curr_x, curr_y, COLOUR_W);
		}
	}
}

void drawRays(SDL_Surface* surface, Circle circle, Circle blocker) {
	std::vector<Ray> rays = generateRaySet(circle);
	for (Ray ray : rays) {
		double curr_x = ray.x_origin;
		double curr_y = ray.y_origin;
		while (curr_x >= 0 && curr_x <= WINDOW_WIDTH && curr_y >= 0 && curr_y <= WINDOW_HEIGHT && !blocker.intersects(curr_x, curr_y)) {
			curr_x += cos(ray.angle);
			curr_y += sin(ray.angle);
			putPixel(surface, curr_x, curr_y, COLOUR_W);
		}
	}
}



int main(int argc, char* argv[]) {

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
	}
	
	SDL_Window* window = SDL_CreateWindow("rt", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	SDL_Surface* surface = SDL_GetWindowSurface(window);

	SDL_Rect blankScreen = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

	Circle rayOrigin = { 200, 200, 50 };
	Circle rayBlocker = { 1300, WINDOW_HEIGHT / 2, 200 };
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
					rayOrigin.x = event.motion.x;
					rayOrigin.y = event.motion.y;

			}
		}
		SDL_FillRect(surface, &blankScreen, COLOUR_BL);
		drawCircle(surface, rayOrigin);
		drawCircle(surface, rayBlocker);
		drawRays(surface, rayOrigin, rayBlocker);
		SDL_UpdateWindowSurface(window);
	}

	SDL_Delay(3000);

	return 0;
	

}