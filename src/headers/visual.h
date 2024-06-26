/*
  "visual.o" library for conveinence and font functions
  Declaration file
  Copyright (C) 2024 Chase Taylor

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Chase Taylor @ creset200@gmail.com
*/

#ifndef LIBRARY_VISUAL
#define LIBRARY_VISUAL

#include <SDL2/SDL_render.h>

extern const int visual_font[];

/**
 * Conveinience function for setting the brush color and drawing a pixel.
 * This varient takes an SDL_Color.
 */
extern void visual_makeDotSDLColor(SDL_Renderer *renderer, int x, int y,
                                   SDL_Color color);

/**
 * Conveinience function for setting the brush color and drawing a pixel.
 * This varient takes a single brightness value, and draws a grey pixel.
 */
extern void visual_makeDotGrayscale(SDL_Renderer *renderer, int x, int y,
                                    Uint8 brightness);

/**
 * Conveinience function for setting the brush color and drawing a pixel.
 * This varient takes individual RGB values.
 */
extern void visual_makeDotRGB(SDL_Renderer *renderer, int x, int y, Uint8 red,
                              Uint8 green, Uint8 blue);

/**
 * Conveinience function for setting the brush color and drawing a pixel.
 * This varient takes individual RGB values, and an alpha value.
 */
extern void visual_makeDotRGBA(SDL_Renderer *renderer, int x, int y, Uint8 red,
                               Uint8 green, Uint8 blue, Uint8 alpha);

/**
 * Draws a character on the screen using the visual font. The font is decided on
 * compile. This takes an index into the font's charset; for conveinience use
 * `visual_charToIdx`.
 */
extern void visual_drawChar(SDL_Renderer *r, int idx, int x, int y,
                            SDL_Color color, int invert);

extern const char visual_charCodes[];

/**
 * Convert an ASCII character to the charset used by the visual font.
 * \returns 1 if no character is found (ususally rendered as a crossed out box
 * that is also explicitly tagged as `'\x1b'`.)
 */
extern char indexes_charToIdx(char c);

/**
 * Prints any string (char*) at the location specified, using the color
 * specified. 
 * \param r The SDL_Renderer to print onto. 
 * \param str The string to print. `visual_charToIdx` is called 
 * internally, so this can just be an ASCII string. 
 * \param x, y The position (Top left corner) to print the text. 
 * \param color The color to print the text with. 
 * \param invert Whether to switch the background and foreground 
 * \param collums Max collums to print the text. When it reaches
 * this many collums it starts a new row (potentially splitting a word)
 */
extern void visual_drawString(SDL_Renderer *r, char *str, int x, int y,
                              SDL_Color color, int invert, int collums);

/**
 * Takes a number and converts it to a string (Base 10).
 * \param str The string to have the number converted to.
 * \param number The number to be converted to a string.
 * \note This does not concatenate to the string; it overwrites it.
 * TODO: Move this function somewhere else
 */
extern void visual_numberToString(char *str, int number);

const SDL_Color visual_blackText = {0, 0, 0, 255};
const SDL_Color visual_redText = {255, 0, 0, 255};
const SDL_Color visual_greenText = {0, 255, 0, 255};
const SDL_Color visual_blueText = {0, 0, 255, 255};
const SDL_Color visual_yellowText = {255, 255, 0, 255};
const SDL_Color visual_greyText = {128, 128, 128, 255};
const SDL_Color visual_whiteText = {255, 255, 255, 255};

#endif