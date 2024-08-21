
#include "SDL2/SDL.h"
#undef main

#include "../lib/QuadTree.h"

#include <string>
#include <random>
#include <ranges>
#include <vector>
#include <algorithm>
#include <chrono>
#include <print>

struct Box {
	NCDS::AABB aabb;
	int value;

	float moveAng;

	auto const& GetAABB() const { return aabb; }

	bool operator==(const Box& other) const {
		return value == other.value;
	}
	bool operator!=(const Box& other) const {
		return value != other.value;
	}
};

void DrawAABB(SDL_Renderer* renderer, const NCDS::AABB& aabb, SDL_Color color) {

	SDL_FRect r;
	r.x = aabb.left;
	r.y = aabb.top;
	r.w = aabb.GetWidth();
	r.h = aabb.GetHeight();
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawRectF(renderer, &r);
}

int main(int argc, char* argv[]) {

	SDL_Init(SDL_INIT_EVERYTHING);

	constexpr float sizeX = 900.f, sizeY = 900.f;
	NCDS::ISpatialContainer<Box>* spatialCont = new NCDS::QuadTree<Box, 5, 5>{NCDS::AABB{0, 0, sizeX, sizeY}};

	auto window = SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 960, {});
	auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	std::uniform_real_distribution dist(0.f, sizeX);
	std::random_device rd;

	std::vector<Box> boxes;

	int cnter = 0;
	for (int i = 0; i < 5000; i++) {
		float x = dist(rd), y = dist(rd);
		auto box = Box{x,y,x +
				  10, y + 10, cnter++, dist(rd)};
		boxes.push_back(box);
		spatialCont->Insert(box);
	}

	bool useQt = true;

	int mouseX = 0, mouseY = 0;
	SDL_Event e;
	while (true) {
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				break;
			else if (e.type == SDL_MOUSEMOTION) {
				mouseX = e.motion.x;
				mouseY = e.motion.y;
			}
			else if (e.type == SDL_MOUSEBUTTONUP) {
				useQt = !useQt;
			}
		}


		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);


		for (auto& box : boxes) { 
			spatialCont->Remove(box);

			float vx = std::cos(box.moveAng),
				vy = std::sin(box.moveAng);
			float nx = box.aabb.left + vx;
			float ny = box.aabb.top + vy;
			if (nx < 0 || nx + 10 > sizeX)
				vx = -vx;
			if (ny < 0 || ny + 10 > sizeY)
				vy = -vy;
			box.moveAng = std::atan2(vy, vx);
			box.aabb.left = std::clamp(box.aabb.left + vx * 0.5f, 0.f, sizeX - 10);
			box.aabb.top = std::clamp(box.aabb.top + vy * 0.5f, 0.f, sizeY - 10);
			box.aabb.right = box.aabb.left + 10;
			box.aabb.bottom = box.aabb.top + 10;

			spatialCont->Insert(box);
		}

		const auto collisionBegin = std::chrono::high_resolution_clock::now();
		if (useQt == false) {
			for (auto& b1 : boxes) {
				bool collided = false;
				for (auto& b2 : boxes) {
					if (b1 != b2 && b1.GetAABB().Intersects(b2.GetAABB())) {
						collided = true;
					}
				}
				if (collided)
					DrawAABB(renderer, b1.GetAABB(), SDL_Color{150,50,50,255});
				else
					DrawAABB(renderer, b1.GetAABB(), SDL_Color{50,150,50,255});
			}
		}
		else {
			for (auto& box : boxes) {
				bool collided = false;
				spatialCont->Query(box.GetAABB(), [&box, &collided] (const auto& other) {
					if (other != box) collided = true;
									  });
				if (collided)
					DrawAABB(renderer, box.GetAABB(), SDL_Color{150,50,50,255});
				else
					DrawAABB(renderer, box.GetAABB(), SDL_Color{50,150,50,255});
			}
		}
		const auto collisionEnd = std::chrono::high_resolution_clock::now();
		auto delta = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(collisionEnd - collisionBegin).count());

		std::println("collision check {:.2f}", delta / 1000.f);

		SDL_RenderPresent(renderer);

	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}

