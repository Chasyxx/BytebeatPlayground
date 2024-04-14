//     This file is part of Bytebeat playground, A stack-based bytebeat runtime.
//     Copyright © 2023, 2024 Chase Taylor

//     Bytebeat Playground is free software: you can redistribute it and/or modify it under
//     the terms of the GNU General Public License as published by the Free Software
//     Foundation, either version 3 of the License, or (at your option) any later
//     version.

//     Bytebeat Playground is distributed in the hope that it will be useful, but WITHOUT ANY
//     WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//     PARTICULAR PURPOSE. See the GNU General Public License for more details.

//     You should have received a copy of the GNU General Public License along with
//     Bytebeat Playground. If not, see <https://www.gnu.org/licenses/>. 

extern "C" {
    #include <visual.h>
}
#include <stdio.h>
#include <SDL2/SDL.h>
// #include <cmath>

void update(SDL_Window *window, SDL_Renderer *renderer)
{
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    const int tinyWidth = windowWidth/8;
    for(int i=0; i<109; i++) {
        const int X = i%tinyWidth;
        const int Y = i/tinyWidth;
        visual_drawChar(renderer,i,X*8,Y*8,visual_whiteText,0);
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

        update(window, renderer);

        // Update the screen
        SDL_RenderPresent(renderer);
    }
}

int main(void)
{
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