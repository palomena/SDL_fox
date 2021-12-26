/******************************************************************************
 *    SDL_fox
 *******************
 * Font rendering library for Simple Direct Media Layer 2.x (SDL2).
 * Copyright (c) 2016-2022 Niklas Benfer <https://github.com/palomena>
 *
 * Dependencies:
 *    freetype2 (https://freetype.org/)
 *    fontconfig (https://www.freedesktop.org/wiki/Software/fontconfig/)
 *    SDL2 (https://www.libsdl.org/)
 *
 * License: MIT License (see ../LICENSE.txt)
 *****************************************************************************/

#ifndef SDL_FOX_H
#define SDL_FOX_H

#include <SDL2/SDL.h>
#include <SDL2/begin_code.h>

/* set up for c function definitions, even when using c++ */
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * SDL_fox library state and initialization
 *****************************************************************************/

/* SDL_fox library initialization state */
enum FOX_LibraryState {
	FOX_UNINITIALIZED,
	FOX_INITIALIZED
};

/* Initializes the SDL_fox library 
 * If the library was initialized previously, then nothing is done. */
extern DECLSPEC enum FOX_LibraryState SDLCALL FOX_Init(void);

/* Deinitializes the SDL_fox library
 * Nothing is done if SDL_fox was not previously initialized.
 */
extern DECLSPEC void SDLCALL FOX_Exit(void);

/* Returns whether SDL_fox has been initialzed. */
extern DECLSPEC enum FOX_LibraryState SDLCALL FOX_WasInit(void);

/******************************************************************************
 * Font definition and open/close
 *****************************************************************************/

/* Internal font representation */
typedef struct FOX_Font FOX_Font;

/* Opens a font via a file-path and specified font parameters. */
extern DECLSPEC FOX_Font* SDLCALL FOX_OpenFont(SDL_Renderer *renderer,
										const char *path, int size);

/* build option to enable fontconfig */
#ifdef FOX_USE_FONTCONFIG

/* Opens a font using a fontconfig string. */
extern DECLSPEC FOX_Font* SDLCALL FOX_OpenFontFc(SDL_Renderer *renderer,
											const unsigned char *fontstr);

#endif /* FOX_USE_FONTCONFIG */

/* Closes a previously opened font via its handle. */
extern DECLSPEC void SDLCALL FOX_CloseFont(FOX_Font *font);

/******************************************************************************
 * Font rendering
 *****************************************************************************/

/* Renders a character at the given position. */
extern DECLSPEC int SDLCALL FOX_RenderChar(FOX_Font *font, Uint32 ch,
						Uint32 previous_ch, const SDL_Point *position);

/* Renders a utf-8 string of text at the given position. */
extern DECLSPEC void SDLCALL FOX_RenderText(FOX_Font *font,
				const Uint8 *text, const SDL_Point *position);

enum FOX_PrintState {
	FOX_ERROR = -1,
	FOX_DONE_PRINTING,
	FOX_NEXT_PAGE,
	FOX_MORE_TEXT
};

/* Renders a utf-8 string of text inside the given rect. */
extern DECLSPEC int SDLCALL FOX_RenderTextInside(FOX_Font *font,
	const Uint8 *text, const Uint8 **endptr, const SDL_Rect *rect, int n);

/* Primarily for debugging purposes, this function renders the entire font
 * atlas at the given position. */
extern DECLSPEC void SDLCALL FOX_RenderAtlas(FOX_Font *font, SDL_Point *pos);

/******************************************************************************
 * Font metrics and glyph dimensions interface
 *****************************************************************************/

/* Specifies the metrics of a character glyph. */
typedef struct {
	SDL_Rect rect;
	SDL_Point bearing;
	int advance;
} FOX_GlyphMetrics;

/* Queries the glyph metrics for a given character. */
extern DECLSPEC const FOX_GlyphMetrics* SDLCALL
FOX_QueryGlyphMetrics(FOX_Font *font, Uint32 ch);

/* Get the x-axis kerning offset for a given character combination. */
extern DECLSPEC int SDLCALL FOX_GetKerningOffset(FOX_Font *font,
								Uint32 ch, Uint32 previous_ch);

/* Get the total x-axis advance spacing for a given character combination. */
extern DECLSPEC int SDLCALL FOX_GetAdvance(FOX_Font *font,
							Uint32 ch, Uint32 previous_ch);

/* Enable/Disable kerning for the specified font. */
extern DECLSPEC void SDLCALL FOX_EnableKerning(FOX_Font *font,
												SDL_bool enable);

/* Specifies the metrics of the font */
typedef struct {
	int height;
	int ptsize;
	int max_width;
	int max_height;
	int max_advance;
} FOX_FontMetrics;

/* Queries the font metrics */
extern DECLSPEC const FOX_FontMetrics* SDLCALL
FOX_QueryFontMetrics(FOX_Font *font);

/* end c function definitions when using c++ */
#ifdef __cplusplus
}
#endif

#include <SDL2/close_code.h>

#endif /* SDL_FOX_H */