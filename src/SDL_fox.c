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

#include "SDL_fox.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <fontconfig.h>

/******************************************************************************
 * SDL_fox library state and initialization
 *****************************************************************************/

static enum FOX_LibraryState FOX_state = FOX_UNINITIALIZED;

static FT_Library libfreetype = NULL;

enum FOX_LibraryState FOX_WasInit(void) {
	return FOX_state;
}

enum FOX_LibraryState FOX_Init(void) {
	if(!FOX_WasInit()) {
		if(!SDL_WasInit(0)) return FOX_state;
		if(!FcInit()) return FOX_state;
		if(FT_Init_FreeType(&libfreetype)) {
			FcFini();
			return FOX_state;
		}

		FOX_state = FOX_INITIALIZED;
	}

	return FOX_state;
}

void FOX_Exit(void) {
	if(FOX_WasInit()) {
		FcFini();
		FT_Done_FreeType(libfreetype);
		FOX_state = FOX_UNINITIALIZED;
	}
}

/******************************************************************************
 * UTF-8 handling
 *****************************************************************************/

typedef struct {
	unsigned char mask;
	unsigned char lead;
	int bits_stored;
} FOX_Utf8;

static FOX_Utf8 *FOX_utf[] = {
	&(FOX_Utf8){0x3f, 0x80, 6},
	&(FOX_Utf8){0x7f,    0, 7},
	&(FOX_Utf8){0x1f, 0xc0, 5},
	&(FOX_Utf8){0xf,  0xe0, 4},
	&(FOX_Utf8){0x7,  0xf0, 3},
	NULL
};

static int FOX_Utf8Length(const char ch) {
	int len = 0;

	for(FOX_Utf8 **u = FOX_utf; *u; ++u) {
		if((ch & ~(*u)->mask) == (*u)->lead) {
			break;
		}
		++len;
	}
	if(len > 4) { /* Malformed leading byte */
		return -1;
	}
	return len;
}

static Uint32 FOX_Utf8Decode(const Uint8* sequence, const Uint8** endptr) {
	int bytes = FOX_Utf8Length(*sequence);
	int shift = FOX_utf[0]->bits_stored * (bytes - 1);
	Uint32 codep = (*sequence++ & FOX_utf[bytes]->mask) << shift;

	for(int i = 1; i < bytes; ++i, ++sequence) {
		shift -= FOX_utf[0]->bits_stored;
		codep |= ((char)*sequence & FOX_utf[0]->mask) << shift;
	}

	*endptr = sequence-1;
	return(codep);
}

/******************************************************************************
 * Font definition and open/close
 *****************************************************************************/

struct FOX_Font {
	SDL_Renderer *renderer;
	SDL_Texture *atlas;
	FOX_GlyphMetrics *metrics;
	FT_Face face;	/* freetype font face */
	int length;		/* side length of the atlas texture (sqrt(width^2)) */
	int size;		/* point size of the font */
	int height;		/* font height */
	SDL_bool use_kerning;
};

FOX_Font* FOX_OpenFont(SDL_Renderer *renderer, const unsigned char *fontstr) {
	FcPattern *pattern = FcNameParse(fontstr);
	if(!pattern) return NULL;

	FcConfigSubstitute(NULL, pattern, FcMatchPattern);
	FcDefaultSubstitute(pattern);
	FcResult result;
	FcPattern *match = FcFontMatch(NULL, pattern, &result);
	if(!match) {
		FcPatternDestroy(pattern);
		return NULL;
	}

	int size;
	FcChar8 *path;
	FcPatternGetString(match, FC_FILE, 0, &path);
	FcPatternGetInteger(match, FC_SIZE, 0, &size);

	FOX_Font *font = FOX_OpenFontEx(renderer, (const char*)path, size);
	FcPatternDestroy(match);
	FcPatternDestroy(pattern);
	return font;
}

static SDL_Surface* FOX_RenderFontToSurface(FOX_Font *font);

FOX_Font* FOX_OpenFontEx(SDL_Renderer *renderer, const char *path, int size) {
	FOX_Font *font = SDL_malloc(sizeof(*font));
	font->renderer = renderer;

	/* Open the font file using libfreetype */
	if(FT_New_Face(libfreetype, path, 0, &font->face)) {
		goto abort0;
	}

	/* Set the pixel size for rendering characters */
	if(FT_Set_Pixel_Sizes(font->face, size, size)) {
		goto abort1;
	}

	/* Calculate atlas surface dimensions */
	int length = (int)SDL_ceil(SDL_sqrt(font->face->num_glyphs));

	/* Set font parameters */
	font->length = length;
	font->size = size;
	font->height = font->face->size->metrics.height >> 6;
	font->use_kerning = FT_HAS_KERNING(font->face);

	/* Render characters to surface */
	SDL_Surface *surface = FOX_RenderFontToSurface(font);
	if(!surface) goto abort1;
	
	/* Convert SDL surface into SDL texture and enable alpha blending */
	font->atlas = SDL_CreateTextureFromSurface(font->renderer, surface);
	SDL_SetTextureBlendMode(font->atlas, SDL_BLENDMODE_BLEND);
	SDL_FreeSurface(surface);

	return font;

	/* Premature error handling */
	abort1:
		FT_Done_Face(font->face);
	abort0:
		SDL_free(font);
		return NULL;
}

void FOX_CloseFont(FOX_Font *font) {
	SDL_DestroyTexture(font->atlas);
	FT_Done_Face(font->face);
	SDL_free(font->metrics);
	SDL_free(font);
}

/*****************************************************************************/

static void FOX_SetMetrics(FOX_Font *font, Uint32 index, int xpos, int ypos) {
	font->metrics[index].rect.x = xpos * font->size;
	font->metrics[index].rect.y = ypos * font->size;
	font->metrics[index].rect.w = font->face->glyph->metrics.width >> 6;
	font->metrics[index].rect.h = font->face->glyph->metrics.height >> 6;
	font->metrics[index].bearing.x = font->face->glyph->metrics.horiBearingX >> 6;
	font->metrics[index].bearing.y = font->face->glyph->metrics.horiBearingY >> 6;
	font->metrics[index].advance = font->face->glyph->metrics.horiAdvance >> 6;
}

SDL_Surface* FOX_RenderFontToSurface(FOX_Font *font) {
	/* Allocate SDL surface */
	int width = font->length * font->size;
	SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, width, width,
												32, SDL_PIXELFORMAT_RGBA32);
	if(!surface) return NULL;

	/* Allocate glyph metrics array */
	font->metrics = SDL_malloc(sizeof(*font->metrics) * font->face->num_glyphs);
	if(!font->metrics) {
		SDL_FreeSurface(surface);
		return NULL;
	}

	FT_UInt index;
	int xpos = 0;
	int ypos = 0;

	for(FT_ULong charcode = FT_Get_First_Char(font->face, &index);
		index != 0;
		charcode = FT_Get_Next_Char(font->face, charcode, &index)
	) {
		if(xpos < (font->length - 1)) {
			xpos++;
		} else {
			xpos = 0;
			ypos++;
		}

		FT_Load_Char(font->face, charcode, FT_LOAD_RENDER);
		FT_Bitmap *bitmap = &font->face->glyph->bitmap;
		if(bitmap->pixel_mode != ft_pixel_mode_grays) {
			break;
		}

		FOX_SetMetrics(font, index, xpos, ypos);

		int xreal = xpos * font->size;
		int yreal = ypos * font->size;
		for(int y = 0; y < bitmap->rows; y++) {
			for(int x = 0; x < bitmap->width; x++) {
				int index = (yreal + y) * surface->w + xreal + x;
				Uint32 *pixel = &((Uint32*)surface->pixels)[index];
				Uint8 alpha = bitmap->buffer[y * bitmap->pitch + x];
				*pixel = SDL_MapRGBA(surface->format, 255, 255, 255, alpha);
			}
		}
	}

	return surface;
}

/******************************************************************************
 * Font rendering
 *****************************************************************************/

static const Uint8* skip_whitespace(const Uint8 *text) {
	while(isspace(*text)) text++;
	return text;
}

static SDL_bool FOX_NextWordFitsOnLine(FOX_Font *font, const Uint8 *text,
									const SDL_Point *position, int maxX
) {
	SDL_Point cursor = *position;
	Uint32 previous_ch = 0;
	for(text = skip_whitespace(text); *text; text++) {
		Uint32 ch = FOX_Utf8Decode(text, &text);
		if(ch == '\n' || ch == '\t' || ch == ' ' || ch == '\r') {
			break;
		} else {
			cursor.x += FOX_GetAdvance(font, ch, previous_ch);
			previous_ch = ch;
		}

		if(cursor.x >= maxX) return SDL_FALSE;
	}

	return SDL_TRUE;
}

static int FOX_RenderLine(FOX_Font *font, const Uint8 *text,
	const Uint8 **endptr, const SDL_Point *position, int width, int n
) {
	int maxX = position->x + width;
	SDL_Point cursor = *position;
	Uint32 previous_ch = 0;
	SDL_bool unsafe = SDL_TRUE;

	/* Skip any whitespace at the beginning of a new line */
	for(text = skip_whitespace(text); *text; text++) {
		if(n == 0) break;

		/* We shall not exceed the line width by printing the next char. */
		if(unsafe && ((cursor.x + font->size) >= maxX)) {
			*endptr = text;
			break;
		}

		Uint32 ch = FOX_Utf8Decode(text, &text);
		if(ch == '\n') {
			continue;
		} else {
			cursor.x += FOX_RenderChar(font, ch, previous_ch, &cursor);
			previous_ch = ch;
			if(ch == ' ') {
				if(!FOX_NextWordFitsOnLine(font, text, &cursor, maxX)) {
					unsafe = SDL_TRUE;
					break;
				} else unsafe = SDL_FALSE;
			}
		}

		if(n > 0) n--;
	}

	*endptr = text;
	return n;
}

/*****************************************************************************/

int FOX_RenderChar(FOX_Font *font, Uint32 ch, Uint32 previous_ch,
										const SDL_Point *position
) {
	int advance = 0;
	const FOX_GlyphMetrics *metrics = FOX_QueryGlyphMetrics(font, ch);
	if(metrics) {
		SDL_Rect dstrect;
		SDL_Color color;

		dstrect.x = position->x;
		dstrect.y = position->y - metrics->bearing.y + font->height;
		dstrect.w = metrics->rect.w;
		dstrect.h = metrics->rect.h;

		if(previous_ch) {
			advance += FOX_GetKerningOffset(font, ch, previous_ch);
			dstrect.x += advance;
		}

		SDL_GetRenderDrawColor(font->renderer, &color.r,
							&color.g, &color.b, &color.a);
		SDL_SetTextureColorMod(font->atlas, color.r, color.g, color.b);
		SDL_RenderCopyEx(font->renderer, font->atlas, &metrics->rect,
								&dstrect, 0.0, NULL, SDL_FLIP_NONE);
		advance += metrics->advance;
	}

	return advance;
}

void FOX_RenderText(FOX_Font *font, const Uint8 *text,
							const SDL_Point *position
) {
	SDL_Point cursor = *position;
	Uint32 previous_ch = 0;
	for(; *text; text++) {
		Uint32 ch = FOX_Utf8Decode(text, &text);
		if(ch == '\n') {
			cursor.x = position->x;
			cursor.y += font->height;
			previous_ch = 0;
			continue;
		} else {
			cursor.x += FOX_RenderChar(font, ch, previous_ch, &cursor);
			previous_ch = ch;
		}
	}
}

int FOX_RenderTextInside(FOX_Font *font, const Uint8 *text,
			const Uint8 **endptr, const SDL_Rect *rect, int n
) {
	int state = 0;
	unsigned linesAvailable = rect->h / font->height;
	if(linesAvailable == 0) {
		return -1;
	}

	#ifdef FOX_DEBUG
	{
		SDL_Color rc;
		SDL_GetRenderDrawColor(font->renderer, &rc.r, &rc.g, &rc.b, &rc.a);
		SDL_SetRenderDrawColor(font->renderer, 255, 255, 255, 255);
		SDL_RenderDrawRect(font->renderer, rect);
		SDL_SetRenderDrawColor(font->renderer, rc.r, rc.g, rc.b, rc.a);
	}
	#endif

	SDL_Point cursor = {rect->x, rect->y};
	for(unsigned line = 0; line < linesAvailable; line++) {
		n = FOX_RenderLine(font, text, &text, &cursor, rect->w, n);
		if(n == 0) {
			state = 1;
			break;
		}

		cursor.x = rect->x;
		cursor.y += font->height;
	}

	if(*text != '\0') {
		*endptr = text;
		return state + 1;
	} else {
		*endptr = NULL;
		return 0;
	}
}

void FOX_RenderAtlas(FOX_Font *font, SDL_Point *pos) {
	SDL_Rect dstrect = {pos->x, pos->y, font->length * font->size,
										font->length * font->size};
	SDL_RenderCopy(font->renderer, font->atlas, NULL, &dstrect);
}

/******************************************************************************
 * Font metrics and glyph dimensions interface
 *****************************************************************************/

const FOX_GlyphMetrics* FOX_QueryGlyphMetrics(FOX_Font *font, Uint32 ch) {
	const FOX_GlyphMetrics *metrics = NULL;
	FT_UInt glyph_index = FT_Get_Char_Index(font->face, ch);
	if(glyph_index != 0) {
		metrics = &font->metrics[glyph_index];
	}

	return metrics;
}

int FOX_GetKerningOffset(FOX_Font *font, Uint32 ch, Uint32 previous_ch) {
	int offset = 0;

	if(font->use_kerning) {
		FT_UInt glyph_index = FT_Get_Char_Index(font->face, ch);
		FT_UInt previous_glyph_index = FT_Get_Char_Index(font->face,
														previous_ch);
		if(glyph_index && previous_glyph_index) {
			FT_Vector delta;
			FT_Get_Kerning(font->face, previous_glyph_index, glyph_index,
											FT_KERNING_DEFAULT, &delta);
			offset = delta.x >> 6;
		}
	}

	return offset;
}

int FOX_GetAdvance(FOX_Font *font, Uint32 ch, Uint32 previous_ch) {
	int advance = 0;
	const FOX_GlyphMetrics *metrics = FOX_QueryGlyphMetrics(font, ch);
	if(metrics) {
		advance += metrics->advance;
		advance += FOX_GetKerningOffset(font, ch, previous_ch);
	}
	return advance;
}

void FOX_EnableKerning(FOX_Font *font, SDL_bool enable) {
	font->use_kerning = enable && FT_HAS_KERNING(font->face);
}