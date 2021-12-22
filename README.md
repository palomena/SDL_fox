# SDL Fox

![SDL logo](data/SDL_logo.png) ![fox](data/fox.png)

**Index:**
- [About](#About)
- [Example code](#Example)
- [Installation](#Installation)
- [Documentation](#Documentation)
- [License](#License)

### About

SDL_Fox is a Utf8 font rendering library for Simple Direct Media Layer 2.x (SDL2). At its core it is a wrapper around the freetype2 library, making use of fontconfig for resolving fonts.

### How does it differ from SDL_ttf?

SDL_ttf is another well-established library one could use to handle fonts with SDL. However SDL_ttf renders text into an SDL_Texture. This means that whenever text changes or is modified it has to create a new texture and destroy the old one. For some applications this may be ok, but for an application like a notepad, which may frequently update a lot of text, it certainly is very cumbersome and counterintuitive to use.

SDL_Fox on the other hand creates a single big texture upon loading the font. Each character contained within the font is rendered to that texture and its position and metrics are stored internally. Whenever text is to be drawn, SDL_Fox uses the position and metrics of each character to find the respective glyph inside the texture. Then it draws the part of the texture containing the character.

For text-intensive applications with lots of volatile text such as word processors, terminal emulators or videogames SDL_Fox provides a significantly easier to use interface - no need to worry about creating and destroying textures each time text is modified. This in turn may increase performance for some applications, although performance is not the issue SDL_Fox is trying to solve (the difference should be neglible in real world use cases). Performance might even be slightly worse than SDL_ttf in some scenarios e.g. for a large batch of constant text. Here SDL_ttf requires only one draw operation (because it just needs to display one texture). SDL_Fox on the other hand needs to iterate over and draw each character. However the disadvantage now becomes apparent - changing just one character of that large chunk of text requires SDL_ttf to create and render a whole new texture, while SDL_Fox does not do anything, except iterate over a new character and use a different index inside its glyph atlas. Keep in mind that the communication between CPU and GPU is slow, so creating and destroying multiple textures every main loop cycle is not as neglible.

Additionally SDL_Fox makes it easier to load fonts according to specified criteria like font name, size and style (bold, italic, etc) by making use of fontconfig. Using fontconfig is optional though, "normal" font loading by path is supported as well.

### Example
```c
#include <SDL2/SDL.h>
#include "SDL_fox.h"

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	FOX_Init();

	SDL_Window *window = SDL_CreateWindow("SDL_Fox example", 
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
								400, 400, SDL_WINDOW_SHOWN);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	FOX_Font *font = FOX_OpenFont(renderer, "DejaVu Sans :style=Bold :size=24");

	for(SDL_bool open = SDL_TRUE; open;) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				open = SDL_FALSE;
			}
		}

		SDL_RenderClear(renderer);
		static const SDL_Point position = {100, 100};
		static const SDL_Color color = {255, 255, 255, 255};
		FOX_RenderText(font, "Hello World!", &position, 0.0f, 0, &color);
		SDL_RenderPresent(renderer);
		SDL_Delay(100);
	}

	FOX_CloseFont(font);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	FOX_Exit();
	SDL_Quit();

	return 0;
}
```

Output
-------
![Screenshot](data/Example.png)

That is a lot of code for displaying a simple *"Hello World"* so let's condense it down to only the SDL_Fox functions we have used:
```c
FOX_Init();
FOX_Font *font = FOX_OpenFont(renderer, "DejaVu Sans :style=Bold :size=24");
FOX_RenderText(font, "Hello World!", &position, 0.0f, 0, &color);
FOX_CloseFont(font);
FOX_Exit();
```

### Installation

#### Install dependencies
1. `apt install -y make build-essential libsdl2-dev libfontconfig-dev libfreetype-dev`
2. You may need to edit the Makefile if you are on anything other than debian linux.
3. Any missing steps or adapation to different systems are given as an exercise to the reader... ;)

#### Build
1. Clone this git repository: `git clone [REPOSITORY URL]`
2. `cd SDL_Fox`
3. `make`
4. `sudo make install` (installs to /usr/local/include not /usr/lib/SDL2)

More detailed instructions can be found in [docs/BUILD.md](docs/BUILD.md).

### Documentation

Documentation can be found in [docs/README.md](docs/README.md).

### License
```
MIT License

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
```