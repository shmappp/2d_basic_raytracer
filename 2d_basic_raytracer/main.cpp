#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <vector>


#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900
#define COLOUR_W 0xffffffff
#define COLOUR_BL 0x00000000
#define NUM_RAYS 500
#define DIRECTION_INCREMENT 5

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

struct Ray {
	double x_origin, y_origin;
	double angle;
};

std::ostream& operator<<(std::ostream& os, const Ray& ray) {
	os << "Ray: x = " << ray.x_origin << ", y = " << ray.y_origin << ", angle (radians) = " << ray.angle << ", angle (deg) = " << ray.angle * 180/M_PI;
	return os;
}



class RayTracer {

private:
	SDL_Window* window;
	SDL_Surface* surface;
	std::vector<Circle*> blockers;
	Circle rayOrigin;
	Circle rayBlocker;
	Circle rayBlocker2;
	int running;

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
			Ray ray = { circle.x, circle.y, angleSlice * (i + 1) };
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

	void drawRays(SDL_Surface* surface, Circle circle, std::vector<Circle*> blockers) {
		std::vector<Ray> rays = generateRaySet(circle);
		for (Ray ray : rays) {
			double curr_x = ray.x_origin;
			double curr_y = ray.y_origin;
			int escape = 0;
			while (curr_x >= 0 && curr_x <= WINDOW_WIDTH && curr_y >= 0 && curr_y <= WINDOW_HEIGHT && !escape) {
				escape = 0;
				for (Circle* blocker : blockers) {
					if ((*blocker).intersects(curr_x, curr_y)) {
						escape = 1;
					}
				}
				if (escape) { break; }
				curr_x += cos(ray.angle);
				curr_y += sin(ray.angle);
				putPixel(surface, curr_x, curr_y, COLOUR_W);
			}
		}
	}

	void processInput() {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = 0;
				break;

			case SDL_MOUSEMOTION:
				rayOrigin.x = event.motion.x;
				rayOrigin.y = event.motion.y;
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_LEFT:
					rayBlocker.x -= DIRECTION_INCREMENT;
					break;

				case SDLK_RIGHT:
					rayBlocker.x += DIRECTION_INCREMENT;
					break;

				case SDLK_UP:
					rayBlocker.y -= DIRECTION_INCREMENT;
					break;

				case SDLK_DOWN:
					rayBlocker.y += DIRECTION_INCREMENT;
					break;

				default:
					break;
				}
			}
		}
	}

	void render() {
		SDL_Rect blankScreen = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
		SDL_FillRect(surface, &blankScreen, COLOUR_BL);
		drawCircle(surface, rayOrigin);
		drawCircle(surface, rayBlocker);
		drawCircle(surface, rayBlocker2);
		drawRays(surface, rayOrigin, blockers);
		SDL_UpdateWindowSurface(window);
	}

	static void mainLoop(void* self) {
		RayTracer* raytracer = static_cast<RayTracer*>(self);
		raytracer->processInput();
		raytracer->render();
	}
public:
	RayTracer()
		: rayOrigin(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 50),
		rayBlocker(1300, WINDOW_HEIGHT / 2, 200),
		rayBlocker2(300, WINDOW_HEIGHT / 2, 200),
		running(1) {
		blockers.push_back(&rayBlocker);
		blockers.push_back(&rayBlocker2);
	}

	bool init() {
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
			return false;
		}
		window = SDL_CreateWindow("rt", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
		if (!window) {
			std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;
			return false;
		}
		surface = SDL_GetWindowSurface(window);
		return true;
	}

	void run() {
		if (!init()) return;
		while (running) {
			processInput();
			render();
		}
	}
};

int main(int argc, char* argv[]) {
	RayTracer raytracer;
	raytracer.run();
	return 0;
}