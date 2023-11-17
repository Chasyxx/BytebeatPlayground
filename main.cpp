//     Bytebeat playground: bytebeat runtime using RPN stack
//     Copyright (C) 2023 Chase T.

//     This program is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.

//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.

//     You should have received a copy of the GNU General Public License
//     along with this program.  If not, see <https://www.gnu.org/licenses/>.

//     creset200@gmail.com

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <string>
#define SAMPLES_SIZE 65536
#define BUFFER_SIZE 512
#define STACK_UNDERFLOW_CHECK()                 \
    if (SP == 0)                                \
    {                                           \
        strcpy(::errorText, "Stack underflow"); \
        break;                                  \
    }
#define STACK_OVERFLOW_CHECK()                 \
    if (SP == 255)                             \
    {                                          \
        strcpy(::errorText, "Stack overflow"); \
        break;                                 \
    }

constexpr SDL_Color WHITE_TEXT{255, 255, 255};
constexpr SDL_Color RED_TEXT{255, 0, 0};
// #define DEBUG

long frame = 0;
long long bigT = 0;
const double PI = 3.141592653589793;

char *input = "Placeholder input";
char *errorText = "Placeholder error";

unsigned char *samples, *samples2;

bool SHIFTKEY = false;
bool CTRLKEY = false;

bool opaqueText = false;
bool waveform = false;

int cursorPos = 0;

int samplesAddition = 0;

#include "./texts.i"

const char charCodes[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

int chartoIdx(char c)
{
    int i = 0;
    while (charCodes[i] != c)
    {
        i++;
        if (i >= 256)
            return 1;
    };
    return i;
}

uint8_t calculateSample(int t)
{
    int PC = 0;
    int *stack = new int[256];
    int SP = 0;
    int val = 0;
    char *vals = "Unknown command !";
    if (stack == nullptr)
    {
        std::cerr << "Memory stack allocation failiure" << std::endl;
        exit(-1);
    }
    stack[0] = t;
    while (::input[PC] != 0)
    {
        bool colon = false;
        switch (::input[PC])
        {
        case 'd':
            STACK_OVERFLOW_CHECK();
            stack[SP + 1] = stack[SP];
            SP++;
            break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            STACK_OVERFLOW_CHECK();
            val = 0;
            while (std::isalnum(::input[PC]) && ::input[PC] < 'G')
            {
                char code = ::input[PC];
                if (code >= 'A')
                    code -= 7;
                val = (val << 4) + code - 48;
                PC++;
            }
            PC--;
            stack[++SP] = val;
            break;

        case 'r':
            STACK_UNDERFLOW_CHECK();
            stack[SP - 1] >>= stack[SP];
            SP--;
            break;
        case 'l':
            STACK_UNDERFLOW_CHECK();
            stack[SP - 1] <<= stack[SP];
            SP--;
            break;
        case '&':
            STACK_UNDERFLOW_CHECK();
            stack[SP - 1] &= stack[SP];
            SP--;
            break;
        case '|':
            STACK_UNDERFLOW_CHECK();
            stack[SP - 1] |= stack[SP];
            SP--;
            break;
        case '^':
            STACK_UNDERFLOW_CHECK();
            stack[SP - 1] ^= stack[SP];
            SP--;
            break;
        case '+':
            STACK_UNDERFLOW_CHECK();
            stack[SP - 1] += stack[SP];
            SP--;
            break;
        case '-':
            STACK_UNDERFLOW_CHECK();
            stack[SP - 1] -= stack[SP];
            SP--;
            break;
        case '*':
            STACK_UNDERFLOW_CHECK();
            stack[SP - 1] *= stack[SP];
            SP--;
            break;
        case '/':
            STACK_UNDERFLOW_CHECK();
            if (stack[SP] == 0)
            {
                stack[SP - 1] = 0;
                SP--;
                break;
            }
            stack[SP - 1] /= stack[SP];
            SP--;
            break;

        case '%':
            STACK_UNDERFLOW_CHECK();
            if (stack[SP] == 0)
            {
                stack[SP - 1] = 0;
                SP--;
                break;
            }
            stack[SP - 1] %= stack[SP];
            SP--;
            break;

        case 'p':
            STACK_UNDERFLOW_CHECK();
            SP--;
            break;

        case 't':
            STACK_OVERFLOW_CHECK();
            stack[++SP] = t;
            break;

        case 'x':
            STACK_UNDERFLOW_CHECK();
            val = stack[SP];
            stack[SP] = stack[SP - 1];
            stack[SP - 1] = val;
            break;

        case '#':
            while (::input[PC] != 0 && ::input[PC] != '\n')
                PC++;
            break;

        case '~':
            stack[SP] = ~stack[SP];
            break;

        case '_':
            stack[SP] = -stack[SP];
            break;

        case '<':
            stack[SP] = SDL_max(0, stack[SP]);
            break;
        case '>':
            stack[SP] = SDL_min(0, stack[SP]);
            break;
        case '=':
            stack[SP] = stack[SP] == 0 ? 1 : 0;
            break;
        case '\\':
            stack[SP] = stack[SP] != 0 ? 1 : 0;
            break;
        case '?':
        {
            STACK_UNDERFLOW_CHECK();
            colon = stack[SP] == 0; // Whether to move to the colon or semicolon
            val = 0;
            PC++;
            if (colon)
                while ((val != 0 || ::input[PC] != ';') && ::input[PC] != 0)
                {
                    if (val < 0)
                        break;
                    if (::input[PC] == '?')
                        val++;
                    if (::input[PC] == ';')
                        val--;
                    if (val == 0 && ::input[PC] == ':')
                    {
                        PC++;
                        break;
                    };
                    PC++;
                }
            if (::input[PC] == 0)
                strcpy(::errorText, "Conditionals must end with ;");
            PC--;
            SP--;
        }
        break;
        case ':':
            val = 0;
            PC++;
            while ((val != 0 || ::input[PC] != ';') && ::input[PC] != 0)
            {
                if (val < 0)
                {
                    strcpy(::errorText, "; with no conditional to match");
                    break;
                }
                if (::input[PC] == '?')
                    val++;
                if (::input[PC] == ';')
                    val--;
                PC++;
            }
            if (::input[PC] == 0)
                strcpy(::errorText, "Conditionals must end with ;");
            break;
        case ';':
        case ' ':
        case '\n':
        case ',':
            break;
        default:
            vals = new char[32];
            strcpy(vals, "Unknown command ");
            vals[16] = ::input[PC];
            vals[17] = '\0';
            strcpy(::errorText, vals);
            delete[] vals;
            break;
        }
        PC++;
    }
    const int result = stack[SP];
    delete[] stack;
    samplesAddition += result;
    return result;
}

namespace audiovisual
{
    const int font[] = {
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

    void drawFontDot(SDL_Renderer *r, int idx, int x, int y, SDL_Color color, bool invert)
    {
        const int X = x % 8;
        const int Y = y % 8;
        // const int XX = X / 8 * 8;
        // const int YY = Y / 8 * 8;
        const int arrayIndex = idx * 8 + Y;
        const bool active = font[arrayIndex] << X & 128 ? invert ? false : true : invert ? true
                                                                                         : false;
        if (active)
            makeDot(r, x, y, color);
        if (!active && ::opaqueText)
            makeDot(r, x, y, color.r ^ 255, color.g ^ 255, color.b ^ 255);
    }

    void drawFont(SDL_Renderer *r, int idx, int x, int y, SDL_Color color, bool invert)
    {
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                drawFontDot(r, idx, x + i, y + j, color, invert);
            }
        }
    }

    void drawVisualization(int windowWidth, int windowHeight, long millis, SDL_Renderer *renderer, long frame)
    {
        const double redMultiplier = SDL_sin(millis / 1000.0 + (PI * 0 / 3)) / 2 + 0.5;
        const double greenMultiplier = SDL_sin(millis / 1000.0 + (PI * 2 / 3)) / 2 + 0.5;
        const double blueMultiplier = SDL_sin(millis / 1000.0 + (PI * 4 / 3)) / 2 + 0.5;
        // Diagram visualization
            for (int pixelY = 0; pixelY < windowHeight; pixelY++)
            {
                for (int pixelX = 0; pixelX < windowWidth; pixelX++)
                {
                    const Uint8 pixel = samples[(pixelX + pixelY * 256) % SAMPLES_SIZE]; //(int)((pixelX - windowWidth / 2.0) * (SDL_fmod((millis / 1000.0), 4.0) - 2)) ^ (int)((pixelY - windowHeight / 2.0) * (SDL_fmod((millis / 1000.0) + 1, 4.0) - 2));
                    makeDot(renderer, pixelX, pixelY, pixel * redMultiplier, pixel * greenMultiplier, pixel * blueMultiplier);
                }
            }
        if (waveform)
        {
            for (int i = 0; i < BUFFER_SIZE; i++)
            {
                unsigned char s1 = 255 - samples2[i];
                unsigned char s2 = 255 - i == 1023 ? 0 : samples2[i + 1];
                bool down = samples2[i] <= samples2[i + 1];
                unsigned char o = static_cast<unsigned char>(SDL_abs(static_cast<int>(samples2[i]) - static_cast<int>(samples2[i + 1])));
                // makeDot(renderer, i&255, s1, 255);
                makeDot(renderer, i / (BUFFER_SIZE/256), s1, 255);
                for (unsigned char j = 1; j < o; j++)
                {
                    makeDot(renderer, i / (BUFFER_SIZE/256), down ? s1 - j : s1 + j, 200);
                }   
            }
        }
        // else
        // {
            
        // }
    }

    void AudioCallback(void *userdata, Uint8 *stream, int len)
    {
        uint8_t *audioBuffer = (uint8_t *)stream;

        // Generate samples
        for (int i = 0; i < len; i++)
        {
            int t = i + ::bigT;
            uint8_t sample = calculateSample(t);
            audioBuffer[i] = sample;
            samples[(i + ::bigT) % SAMPLES_SIZE] = sample;
            samples[(i + ::bigT + 256) % SAMPLES_SIZE] = 0;
            samples[(i + ::bigT + 512) % SAMPLES_SIZE] = 128;
            samples[(i + ::bigT + 768) % SAMPLES_SIZE] = 255;
        }
        ::bigT += len;
        for (int i = 0; i < BUFFER_SIZE; i++)
            samples2[i] = audioBuffer[i];
    }
};

void update(SDL_Window *window, SDL_Renderer *renderer, long frame)
{
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    long millis = SDL_GetTicks64();
    audiovisual::drawVisualization(windowWidth, windowHeight, millis, renderer, frame);
    int charIdx = 0;
    char row = 0;
    char col = 0;
    while (::input[charIdx] != 0)
    {
        if (::input[charIdx] == '\n')
        {
            if (cursorPos == charIdx)
            {
                audiovisual::drawFont(renderer, 0, col * 8, row * 8, WHITE_TEXT, 1);
            }
            col = 0;
            row++;
            charIdx++;
        }
        else
        {
            audiovisual::drawFont(renderer, chartoIdx(::input[charIdx++]), col * 8, row * 8, WHITE_TEXT, cursorPos == charIdx);
            col++;
            if (col == 32)
            {
                col = 0;
                row++;
            }
        }
    }
    if (cursorPos == charIdx)
    {
        audiovisual::drawFont(renderer, 0, col * 8, row * 8, WHITE_TEXT, 1);
    }
    charIdx = 0;
    while (::errorText[charIdx] != 0)
    {
        audiovisual::drawFont(renderer, chartoIdx(::errorText[charIdx++]), charIdx * 8, 248, RED_TEXT, 0);
    }
}

char handleKey(int key)
{
#ifdef DEBUG
    std::cout << "key `" << SDL_GetKeyName(key) << "` pressed (" << key << ") [SHIFT " << ::SHIFTKEY << "]" << std::endl;
#endif
    if (key >= 'a' && key <= 'z' && ::SHIFTKEY)
        key -= 32; // if (SP == 255)
                   //     break;
                   // stack[++SP] = 15;
                   // break;
    switch (key)
    {
    case SDLK_RETURN:
    case SDLK_RETURN2:
        key = '\n';
        break;
    }
    if (::SHIFTKEY)
        switch (key)
        {
            break;
        case '1':
            key = '!';
            break;
        case '2':
            key = '@';
            break;
        case '3':
            key = '#';
            break;
        case '4':
            key = '$';
            break;
        case '5':
            key = '%';
            break;
        case '6':
            key = '^';
            break;
        case '7':
            key = '&';
            break;
        case '8':
            key = '*';
            break;
        case '9':
            key = '(';
            break;
        case '0':
            key = ')';
            break;

        case '`':
            key = '~';
            break;
        case '-':
            key = '_';
            break;
        case '=':
            key = '+';
            break;
        case '[':
            key = '{';
            break;
        case ']':
            key = '}';
            break;

        case '\\':
            key = '|';
            break;
        case ',':
            key = '<';
            break;
        case '.':
            key = '>';
            break;
        case '/':
            key = '?';
            break;

        case ';':
            key = ':';
            break;
        case '\'':
            key = '"';
            break;
        }
    return key;
}

void SDLMainLoop(SDL_Renderer *renderer, SDL_Window *window, SDL_AudioSpec &ASPEC)
{
    bool quit = false;
    SDL_Event event;

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                // Check the key that was pressed
                SDL_Keysym keysym = event.key.keysym;
                SDL_Keycode keycode = keysym.sym;
                if (::CTRLKEY)
                {
                    if (keycode == 't')
                    {
                        opaqueText = !opaqueText;
                    }
                    else if (keycode == 'w')
                    {
                        waveform = !waveform;
                    }
                    else if (keycode == 's')
                    {
                        if (::input[0] == '#' && ::input[1] == '#')
                        {
                            int endIndex = 2;
                            while (::input[endIndex] != '\n' && ::input[endIndex] != '\0')
                                endIndex++;
                            if (::input[endIndex] == '\0')
                                continue;
                            char *buffer = new char[256];
                            for (int i = 0; i < endIndex - 2; i++)
                            {
                                buffer[i] = ::input[i + 2];
                            }
                            buffer[endIndex - 2] = '\0'; // Null terminator
                            std::ofstream file(buffer);
                            delete[] buffer;
                            buffer = new char[1024];
                            int i = 0;
                            while (::input[i] != 0)
                                i++;
                            std::cout << std::boolalpha << file.is_open() << buffer << endIndex << std::endl;
                            if (file.is_open())
                            {
                                file.write(::input, i);
                            }
                            delete[] buffer;
                        }
                        else
                        {
                            char *buffer = new char[1024];
                            for (int i = 0; i < 1024; i++)
                                buffer[i] = 0;
                            strcat(buffer, "##file.bp\n\n");
                            strcat(buffer, ::input);
                            strcpy(::input, buffer);
                            delete[] buffer;
                            continue;
                        }
                    }
                }
                else
                {
                    if (keycode == SDLK_ESCAPE)
                    {
                        quit = 1; // Quit the program when the Escape key is pressed
                    }
                    else if (keycode == SDLK_BACKSPACE)
                    {
                        if (cursorPos == 0)
                            continue;
                        const int len = strlen(::input);
                        char *substr = new char[len - cursorPos + 1];
                        for (int i = 0; i < len - cursorPos; i++)
                        {
                            substr[i] = ::input[i + cursorPos];
                        }
                        substr[len - cursorPos] = 0; // Null terminator
                        for (int i = 0; i < len - cursorPos + 1; i++)
                        {
                            ::input[i + cursorPos - 1] = substr[i];
                        }
                        delete[] substr;
                        cursorPos = SDL_max(0, cursorPos - 1);
                        strcpy(::errorText, "");
                    }
                    else if (keycode == SDLK_F1)
                    {
                        SDL_PauseAudio(1);
                    }
                    else if (keycode == SDLK_F2)
                    {
                        ::frame = ::bigT = 0;
                    }
                    else if (keycode == SDLK_F3)
                    {
                        SDL_PauseAudio(0);
                    }
                    else if (keycode == SDLK_LSHIFT || keycode == SDLK_RSHIFT)
                    {
                        ::SHIFTKEY = true;
                    }
                    else if (keycode == SDLK_LEFT)
                    {
                        cursorPos = SDL_max(0, cursorPos - 1);
                    }
                    else if (keycode == SDLK_RIGHT)
                    {
                        cursorPos = SDL_min(strlen(::input), cursorPos + 1);
                    }
                    else if (keycode == SDLK_UP)
                    {
                        cursorPos = SDL_max(0, cursorPos - 8);
                    }
                    else if (keycode == SDLK_DOWN)
                    {
                        cursorPos = SDL_min(strlen(::input), cursorPos + 8);
                    }
                    else if (keycode == SDLK_LCTRL || keycode == SDLK_RCTRL)
                    {
                        ::CTRLKEY = true;
                    }
                    else if (keycode == SDLK_LALT || keycode == SDLK_RALT)
                    {
                    }
                    else
                    {
                        const char key = handleKey(keycode);
                        const int len = strlen(::input);
                        if (cursorPos == len)
                        {
                            ::input[len] = key;
                        }
                        else
                        {
                            char *substr = new char[len - cursorPos + 1];
                            for (int i = 0; i < len - cursorPos; i++)
                            {
                                substr[i] = ::input[i + cursorPos];
                            }
                            substr[len - cursorPos] = 0; // Null terminator
                            ::input[cursorPos] = key;
                            for (int i = 0; i < len - cursorPos + 1; i++)
                            {
                                ::input[i + cursorPos + 1] = substr[i];
                            }
                            delete[] substr;
                        }
                        cursorPos = SDL_min(strlen(::input), cursorPos + 1);
                        strcpy(::errorText, "");
                    }
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                SDL_Keysym keysym = event.key.keysym;
                SDL_Keycode keycode = keysym.sym;
                if (keycode == SDLK_LSHIFT || keycode == SDLK_RSHIFT)
                {
                    ::SHIFTKEY = false;
                }
                else if (keycode == SDLK_LCTRL || keycode == SDLK_RCTRL)
                {
                    ::CTRLKEY = false;
                }
            }
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        update(window, renderer, ::frame++);

        // Update the screen
        SDL_RenderPresent(renderer);
    }
}

int main(int argc, char **argv)
{
    std::cout << version << std::endl;
    std::cout << "Initial initialization..." << std::endl;
    ::input = new char[1024];
    ::errorText = new char[32];
    if (::input == nullptr)
    {
        std::cerr << "Failed to allocate the memory heap.";
        return -1;
    }
    if (::errorText == nullptr)
    {
        std::cerr << "Failed to allocate the memory heap.";
        return -1;
    }
    samples = new unsigned char[SAMPLES_SIZE];
    samples2 = new unsigned char[BUFFER_SIZE];
    if (samples == nullptr)
    {
        std::cerr << "Failed to allocate the memory heap.";
        return -1;
    }
    for (int i = 0; i < argc; i++)
    {
        std::cout << argv[i] << std::endl;
    }
    std::cout << "Initialazing SDL...\n";
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        std::cerr << "Failed to initialize SDL.";
        return -1;
    }
    std::cout << note << std::endl;
    std::cout << "Enter your samplerate." << std::endl;
    int SR;
    scanf("%d", &SR);
    std::cout << "Opening window..." << std::endl;
    SDL_Window *window = SDL_CreateWindow(version, 0, 0, 256, 256, SDL_WINDOW_SHOWN); //  | SDL_WINDOW_RESIZABLE
    if (window == nullptr)
    {
        std::cerr << "Failed to spawn window." << std::endl;
        return -1;
    }
    std::cout << "Creating renderer..." << std::endl;
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        std::cerr << "Failed to create a renderer." << std::endl;
        return -1;
    }
    std::cout << "Starting!\n";

    SDL_AudioSpec audioSpec;

    audioSpec.freq = SR;
    audioSpec.format = AUDIO_U8;
    audioSpec.channels = 1;
    audioSpec.samples = BUFFER_SIZE;
    audioSpec.callback = audiovisual::AudioCallback;
    if (argc > 1)
    {
        const char *filePath = argv[1];
        std::ifstream file(filePath);
        if (file.is_open())
        {
            file.read(::input, 1024);
        }
        else
        {
            char *error = new char[1024];
            for (int i = 0; i < 1024; i++)
                error[i] = 0;
            strcat(error, "# Couldn't open the file:\n# ");
            strcat(error, argv[1]);
            strcat(error, "\n\ntFr1&?t6_*7/:t10r1&?t9*64&:t64&9_*;B/;tt5*&t6r||A*~");
            ::opaqueText = true;
            strcpy(::input, error);
            delete[] error;
        }
    }
    else
    {
        char *defaultInput = new char[1024];
        for (int i = 0; i < 1024; i++)
            defaultInput[i] = 0;
        if (defaultInput == nullptr)
        {
            std::cerr << "Failed to allocate the memory heap.";
            return -1;
        }
        strcat(defaultInput, "# ");
        strcat(defaultInput, version);
        strcat(defaultInput, "\n\ntFr1&?t6_*7/:t10r1&?t9*64&:t64&9_*;B/;tt5*&t6r||A*~");
        strcpy(::input, defaultInput);
        delete[] defaultInput;
    }

    if (SDL_OpenAudio(&audioSpec, NULL) < 0)
    {
        fprintf(stderr, "Failed to open audio: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    // Start audio playback
    SDL_PauseAudio(0);

    SDLMainLoop(renderer, window, audioSpec);

    // Clean up and exit
    printf("Exiting...\n");
    SDL_CloseAudio();
    SDL_Quit();

    return 0;
}