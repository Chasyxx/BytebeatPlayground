//     This file is part of Bytebeat playground, A stack-based bytebeat runtime.
//     Copyright © 2023 Chase T.

//     Bytebeat Playground is free software: you can redistribute it and/or modify it under
//     the terms of the GNU General Public License as published by the Free Software
//     Foundation, either version 3 of the License, or (at your option) any later
//     version.

//     Bytebeat Playground is distributed in the hope that it will be useful, but WITHOUT ANY
//     WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//     PARTICULAR PURPOSE. See the GNU General Public License for more details.

//     You should have received a copy of the GNU General Public License along with
//     Bytebeat Playground. If not, see <https://www.gnu.org/licenses/>. 

#include <stdio.h>
#include <SDL2/SDL.h>
#include <string>
// #include <cmath>

long frame = 0;
long long bigT = 0;
const double PI = 3.141592653589793;

const Uint8 font[] = {
    #include "font.ipp"
};

void makeDot(SDL_Renderer *renderer, int x, int y, SDL_Color color)
{
    const SDL_Rect *rect = new SDL_Rect{x, y, 1, 1};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawRect(renderer, rect);
    delete rect;
}

void makeDot(SDL_Renderer *renderer, int x, int y, Uint8 brightness)
{
    const SDL_Rect *rect = new SDL_Rect{x, y, 1, 1};
    SDL_SetRenderDrawColor(renderer, brightness, brightness, brightness, 255U);
    SDL_RenderDrawRect(renderer, rect);
    delete rect;
}

void makeDot(SDL_Renderer *renderer, int x, int y, Uint8 red, Uint8 green, Uint8 blue)
{
    const SDL_Rect *rect = new SDL_Rect{x, y, 1, 1};
    SDL_SetRenderDrawColor(renderer, red, green, blue, 255U);
    SDL_RenderDrawRect(renderer, rect);
    delete rect;
}

void makeDot(SDL_Renderer *renderer, int x, int y, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
{
    const SDL_Rect *rect = new SDL_Rect{x, y, 1, 1};
    SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
    SDL_RenderDrawRect(renderer, rect);
    delete rect;
}

void drawVisualization(int windowWidth, int windowHeight, long millis, SDL_Renderer *renderer, long frame)
{
    for (int pixelY = 0; pixelY < windowHeight; pixelY++)
    {
        for (int pixelX = 0; pixelX < windowWidth; pixelX++)
        {
            const double redMultiplier = SDL_sin(frame / 100.0 + (PI * 0 / 3)) / 2 + 0.5;
            const double greenMultiplier = SDL_sin(frame / 100.0 + (PI * 1 / 3)) / 2 + 0.5;
            const double blueMultiplier = SDL_sin(frame / 100.0 + (PI * 2 / 3)) / 2 + 0.5;
            const Uint8 pixel = (int)((pixelX - windowWidth / 2.0) * SDL_fabs(SDL_fmod((millis / 1000.0), 4.0) - 2)) ^ (int)((pixelY - windowHeight / 2.0) * SDL_fabs(SDL_fmod((millis / 1000.0) + 1, 4.0) - 2));
            makeDot(renderer, pixelX, pixelY, pixel * redMultiplier, pixel * greenMultiplier, pixel * blueMultiplier);
        }
    }
}

void drawFontDot(SDL_Renderer *r,int idx,int x,int y) {
    const int X = x % 8;
    const int Y = y % 8;
    // const int XX = X / 8 * 8;
    // const int YY = Y / 8 * 8;
    const int arrayIndex = idx*8+Y;
    makeDot(r,x,y,font[arrayIndex]<<X&128?255:0);
}

void drawFont(SDL_Renderer *r, int idx, int x, int y) {
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            drawFontDot(r,idx,x+i,y+j);
        }
    }
}

void update(SDL_Window *window, SDL_Renderer *renderer, long frame)
{
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    long millis = SDL_GetTicks64();
    drawVisualization(windowWidth, windowHeight, millis, renderer, frame);
    const int tinyWidth = windowWidth/8;
    for(int i=0; i<96; i++) {
        const int X = i%tinyWidth;
        const int Y = i/tinyWidth;
        drawFont(renderer,i,X*8,Y*8);
    }
}

void SDLMainLoop(SDL_Renderer *renderer, SDL_Window *window)
{
    bool quit = false;
    SDL_Event event;

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                printf("Exiting...\n");
                quit = true;
            }
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        update(window, renderer, frame++);

        // Update the screen
        SDL_RenderPresent(renderer);
    }
}

int main(void)
{
    printf("Hello, world!\n");
    printf("Initilazing SDL...\n");
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Failed to initialize SDL.");
        return -1;
    }
    printf("Opening window...\n");
    SDL_Window *window = SDL_CreateWindow("SDL Window", 0, 0, 256, 256, SDL_WINDOW_SHOWN); //  | SDL_WINDOW_RESIZABLE
    if (window == nullptr)
    {
        printf("Failed to spawn window.");
        return -1;
    }
    printf("Creating renderer...\n");
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        printf("Failed to create a renderer.");
        return -1;
    }
    printf("Starting!\n");

    SDLMainLoop(renderer, window);

    SDL_Quit();

    return 0;
}