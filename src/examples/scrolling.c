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
	const Uint8 text[] = {
		"Hello World! This is a short message... "
		"We are currently testing the SDL_fox font "
		"rendering library. Specifically the "
		"FOX_RenderTextInside() function. Let's see "
		"if this works."
	};
	const Uint8 *textptr = text;
	const Uint8 *endptr = NULL;
	Uint32 ticks = SDL_GetTicks();
	enum FOX_PrintState printstate = FOX_DONE_PRINTING;
	int n = 0;
	SDL_bool swap = SDL_FALSE;

	for(SDL_bool open = SDL_TRUE; open;) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				open = SDL_FALSE;
			} else if(event.type == SDL_KEYUP && printstate < FOX_MORE_TEXT) {
				if(printstate > 0) textptr = endptr;
				else textptr = text;
				n = 0;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		static const SDL_Rect position = {75, 100, 375, 30*4};
		SDL_SetRenderDrawColor(renderer, 255, 255, 150, 255);
		printstate = FOX_RenderTextInside(font, textptr, &endptr, &position, n);

		Uint32 cticks = SDL_GetTicks();
		if((cticks - ticks) > 100) {
			ticks = cticks;
			printf("n: %d\n", n);
			if(printstate == FOX_MORE_TEXT) n++;
			swap = !swap;
		}

		if(printstate < FOX_MORE_TEXT && swap) {
			SDL_Rect rect = position;
			if(printstate > 0)
				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			else
				SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
			rect.x -= 10;
			rect.y -= 10;
			rect.w += 20;
			rect.h += 20;
			SDL_RenderDrawRect(renderer, &rect);
		}

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