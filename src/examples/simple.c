#include <SDL2/SDL.h>
#include <stdio.h>
#include "SDL_fox.h"

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	FOX_Init();

	SDL_Window *window = SDL_CreateWindow("SDL_Fox example", 
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
								600, 400, SDL_WINDOW_SHOWN);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	FOX_Font *font = FOX_OpenFont(renderer, "./OpenSans-Light.ttf", 24);
	if(!font) {
		puts("Failed to open font file!");
		return -1;
	}

	for(SDL_bool open = SDL_TRUE; open;) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				open = SDL_FALSE;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		static const SDL_Point position = {100, 100};
		SDL_SetRenderDrawColor(renderer, 255, 255, 150, 255);
		FOX_RenderText(font, "Hello World!\nSDL_fox src/examples/simple.c", &position);
		SDL_RenderPresent(renderer);
		SDL_Delay(10);
	}

	FOX_CloseFont(font);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	FOX_Exit();
	SDL_Quit();

	return 0;
}