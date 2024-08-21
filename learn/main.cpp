
#include "SDL2/SDL.h"
#undef main


int main(int argc, char* argv[]) {

	SDL_Init(SDL_INIT_EVERYTHING);


	auto window = SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 960, {});
	auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	SDL_Event e;
	while (true) {
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				break;
		}


		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_RenderPresent(renderer);

	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}

