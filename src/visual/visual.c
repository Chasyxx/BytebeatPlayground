/*
  "visual.o" library for conveinence and font functions
  Implementation file
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

#include <SDL2/SDL_render.h>
#include <limits.h>

const int visual_font[] = {
  #include "./font.i" // This file will be generated by Perl, run `make font` if you get an error from your language server
};

void visual_makeDotSDLColor(SDL_Renderer *renderer, int x, int y,
                            SDL_Color color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderDrawPoint(renderer, x, y);
}

void visual_makeDotGrayscale(SDL_Renderer *renderer, int x, int y,
                             Uint8 brightness) {
  SDL_SetRenderDrawColor(renderer, brightness, brightness, brightness, 255U);
  SDL_RenderDrawPoint(renderer, x, y);
}

void visual_makeDotRGB(SDL_Renderer *renderer, int x, int y, Uint8 red,
                       Uint8 green, Uint8 blue) {
  SDL_SetRenderDrawColor(renderer, red, green, blue, 255U);
  SDL_RenderDrawPoint(renderer, x, y);
}

void visual_makeDotRGBA(SDL_Renderer *renderer, int x, int y, Uint8 red,
                        Uint8 green, Uint8 blue, Uint8 alpha) {
  SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
  SDL_RenderDrawPoint(renderer, x, y);
}

void visual_drawFontDot(SDL_Renderer *r, int idx, int x, int y, int X, int Y,
                        SDL_Color color, int invert) {
  const int arrayIndex = idx * 8 + Y;
  const int active = visual_font[arrayIndex] << X & 128 ? invert ? 0 : 1
                     : invert                            ? 1
                                                         : 0;
  if (active)
    visual_makeDotSDLColor(r, x+X, y+Y, color);
}

void visual_drawChar(SDL_Renderer *r, int idx, int x, int y, SDL_Color color,
                     int invert) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      visual_drawFontDot(r, idx, x, y, i, j, color, invert);
    }
  }
}

const char visual_charCodes[] =
#include "./font.charset"
    ;

char indexes_charToIdx(char c) {
  unsigned short i = 0;
  while (visual_charCodes[i] != c) {
    i++;
    if (i > UCHAR_MAX)
      return 1;
  };
  return (char)(i);
}

void visual_drawString(SDL_Renderer *r, char *str, int x, int y,
                       SDL_Color color, int invert, int collums) {
  int i = 0;
  int X = 0;
  int Y = 0;
  while (str[i] != 0) {
    if (str[i] == '\n') {
      X = 0;
      Y++;
    } else {
      visual_drawChar(r, indexes_charToIdx(str[i]), x + (X * 8), y + (Y * 8),
                      color, invert);
      X++;
    }
    if (X == collums) {
      X = 0;
      Y++;
    }
    i++;
  }
}

void visual_numberToString(char *str, int number) {
  if (number == 0) {
    str[0] = '0';
    str[1] = '\0';
    return;
  }
  int neg = number < 0;
  number = SDL_fabs(number);
  int n = (int)(SDL_floor(SDL_log10(number)));
  str[n + 1] = '\0';
  for (int i = n; i >= 0; i--) {
    str[i + neg] = 48 + (number % 10);
    number /= 10;
  }
  if (neg)
    str[0] = '-';
}
