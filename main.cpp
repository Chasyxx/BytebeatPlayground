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

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <string>
#include <cstring>
#include <errno.h>
#include <vector>
#include <filesystem>
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
namespace colors
{
    constexpr SDL_Color white{255, 255, 255};
    constexpr SDL_Color black{0, 0, 0};
    constexpr SDL_Color red{255, 0, 0};
    constexpr SDL_Color green{0, 255, 0};
};
// #define DEBUG

long frame = 0;
long long bigT = 0;
const double PI = 3.141592653589793;

constexpr int memory_size = 1 << 22; // 4 MiB of memory allocated

char *input = "Placeholder input";
char *errorText = "Placeholder error";
int *memory;

char* argv0;

void deleteGlobals()
{
    for (int i = 0; i < memory_size; i++)
    {
        ::memory[i] = 0;
    }
}

unsigned char *samples, *samples2;

bool SHIFTKEY = false;
bool CTRLKEY = false;

namespace drawSettings
{
    bool waveform = true;
}

int cursorPos = 0;

#include "./texts.i"

const char charCodes[] = " \x1b!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

inline int mod(int a, int b) { return (a % b + b) % b; };

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

inline bool isHex(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

char hexToChar(char c)
{
    if (isHex(c))
    {
        return (c >= 'A') ? c - 55 : c - 48;
    }
    else
    {
        strcpy(::errorText, "Badly formatted hex");
        return 0;
    }
}

uint8_t calculateSample(int t)
{
    int PC = 0;
    int *stack = new int[256];
    std::vector<int> callStack;
    int SP = 0;
    int val = 0;
    std::vector<char> vec;
    char *vals = "Unknown command \x1b";
    bool inString = false;
    std::vector<int> loopPoints;
    std::vector<int> loopCounts;
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
            while (isHex(::input[PC]))
            {
                val = (val << 4) + hexToChar(::input[PC]);
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
                while ((val != 0 || ::input[PC] != ';' || inString) && ::input[PC] != 0)
                {
                    if (::input[PC] == '"' && !(inString && ::input[PC - 1] == '\\'))
                    {
                        inString = !inString;
                    }
                    if (val < 0)
                        break;
                    if (::input[PC] == '?' && !inString)
                        val++;
                    if (::input[PC] == ';' && !inString)
                        val--;
                    if (val == 0 && ::input[PC] == ':' && !inString)
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
                if (::input[PC] == '"' && !(inString && ::input[PC - 1] == '\\'))
                {
                    inString = !inString;
                }
                if (::input[PC] == '?' && !inString)
                    val++;
                if (::input[PC] == ';' && !inString)
                    val--;
                PC++;
            }
            if (::input[PC] == 0)
                strcpy(::errorText, "Conditionals must end with ;");
            break;
        case '[':
        {
            vec = std::vector<char>();
            PC++;
            if (::input[PC] == ',')
                PC++;
            while (::input[PC] != ']')
            {
                if (::input[PC + 1] == ']' || ::input[PC + 1] == '\0' || ::input[PC] == '\0')
                {
                    strcpy(::errorText, "Badly formatted constantArray");
                    PC++;
                    break;
                }
                vec.push_back((hexToChar(::input[PC]) << 4) + hexToChar(::input[PC + 1]));
                PC += 2;
                if (::input[PC] == ',')
                    PC++;
            }
            val = vec.size();
            if (val == 0)
            {
                strcpy(::errorText, "Badly formatted constantArray");
                break;
            }
            try
            {
                stack[SP] = vec.at(stack[SP] % val);
            }
            catch (std::out_of_range e)
            {
                strcpy(::errorText, "constantArray OOR error");
            }
            break;
        }
        case '"':
        {
            vec = std::vector<char>();
            PC++;
            while (::input[PC] != '"')
            {
                if (::input[PC] == '\0')
                {
                    strcpy(::errorText, "Badly formatted constantString");
                    PC++;
                    break;
                }
                if (::input[PC] == '\\')
                {
                    PC++;
                    vec.push_back(::input[PC]);
                }
                else
                    vec.push_back(::input[PC]);
                PC++;
            }
            val = vec.size();
            if (val == 0)
            {
                strcpy(::errorText, "Badly formatted constantString");
                break;
            }
            try
            {
                stack[SP] = vec.at((stack[SP] % val + val) % val);
            }
            catch (std::out_of_range e)
            {
                strcpy(::errorText, "constantString OOR error");
            }
            break;
        }
        case '!':
        case 'W':
            if (SP < 2)
            {
                strcpy(::errorText, "Stack underflow x2");
                break;
            }
            ::memory[mod(stack[SP], memory_size)] = stack[SP - 1];
            SP -= 2;
            break;
        case '@':
        case 'R':
            stack[SP] = ::memory[mod(stack[SP], memory_size)];
            break;
        case '{':
            STACK_UNDERFLOW_CHECK();
            ::memory[mod(stack[SP], memory_size)] = PC;
            while (::input[PC] != '}' || inString)
            {
                PC++;
                if (::input[PC] == '"' && !(inString && ::input[PC - 1] == '\\'))
                {
                    inString = !inString;
                }
                if (::input[PC] == '\0')
                {
                    strcpy(::errorText, "Functions must end with }");
                    break;
                }
            }
            SP--;
            break;
        case 'f':
            STACK_UNDERFLOW_CHECK();
            callStack.push_back(PC);
            PC = ::memory[mod(stack[SP], memory_size)];
            SP--;
            break;
        case '}':
            if (callStack.size() == 0)
            {
                strcpy(::errorText, "Call stack underflow");
                break;
            }
            PC = callStack.back();
            callStack.pop_back();
            break;
        case 'L':
            STACK_UNDERFLOW_CHECK();
            val = 0;
            if (stack[SP]==0)  {
                PC++;
                while ((val != 0 || ::input[PC] != 'e') && ::input[PC] != 0)
                {
                    if (::input[PC] == '"' && !(inString && ::input[PC - 1] == '\\'))
                    {
                        inString = !inString;
                    }
                    if (val < 0)
                        break;
                    if (::input[PC] == 'L' && !inString)
                        val++;
                    if (::input[PC] == 'e' && !inString)
                        val--;
                    PC++;
                }
                if(::input[PC]==0) {
                    strcpy(::errorText, "Loops must end with e");
                }
            } else {
                loopCounts.push_back(stack[SP]);
                loopPoints.push_back(PC);
            }
            SP--;
            break;
        case 'e':
            if(loopCounts.size() == 0) {
                strcpy(::errorText, "Loop stack underflow");
                break;
            }
            if(loopPoints.size() == 0) {
                strcpy(::errorText, "Loop stack underflow");
                break;
            }
            if(loopCounts.back() < 2) {
                loopCounts.pop_back();
                loopPoints.pop_back();
                break;
            }
            val = loopCounts.back();
            loopCounts.pop_back();
            loopCounts.push_back(val-1);
            PC = loopPoints.back();
             break;
        case 'X':
            if(loopPoints.size() == 0) {
                strcpy(::errorText, "Loop stack underflow (break)");
                break;
            }
            loopCounts.pop_back();
            loopPoints.pop_back();
            while ((val != 0 || ::input[PC] != 'e') && ::input[PC] != 0)
                {
                    if (val < 0)
                        break;
                    if (::input[PC] == 'L')
                        val++;
                    if (::input[PC] == 'e')
                        val--;
                    PC++;
                }
                if(::input[PC]==0) {
                    strcpy(::errorText, "Loops must end with e");
                }
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
    return result;
}

namespace audiovisual
{
    const int font[] = {
        #include "font.ipp"
    };

    void makeDot(SDL_Renderer *renderer, int x, int y, SDL_Color color)
    {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderDrawPoint(renderer, x, y);
    }

    void makeDot(SDL_Renderer *renderer, int x, int y, Uint8 brightness)
    {
        SDL_SetRenderDrawColor(renderer, brightness, brightness, brightness, 255U);
        SDL_RenderDrawPoint(renderer, x, y);
    }

    void makeDot(SDL_Renderer *renderer, int x, int y, Uint8 red, Uint8 green, Uint8 blue)
    {
        SDL_SetRenderDrawColor(renderer, red, green, blue, 255U);
        SDL_RenderDrawPoint(renderer, x, y);
    }

    void makeDot(SDL_Renderer *renderer, int x, int y, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
    {
        SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
        SDL_RenderDrawPoint(renderer, x, y);
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
        for (int idx = 0; idx < 65536; idx++)
        {
            int x = idx >> 8;
            int y = idx & 255;
            makeDot(renderer, x, y, (int)(samples[idx] * redMultiplier), (int)(samples[idx] * greenMultiplier), (int)(samples[idx] * blueMultiplier));
        }
        if (drawSettings::waveform)
        {
            for (int i = 0; i < BUFFER_SIZE; i++)
            {
                unsigned char s1 = 255 - ::samples2[i];
                bool down = ::samples2[i] <= ::samples2[i + 1];
                unsigned char o = static_cast<unsigned char>(SDL_abs(static_cast<int>(::samples2[i]) - static_cast<int>(::samples2[i + 1])));
                // makeDot(renderer, i&255, s1, 255);
                makeDot(renderer, (int)(i / (BUFFER_SIZE / 512.0)), s1 + 256, 255);
                for (unsigned char j = 1; j < o; j++)
                {
                    makeDot(renderer, (int)(i / (BUFFER_SIZE / 512.0)), (down ? s1 - j : s1 + j) + 256, 200);
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
            ::samples[(i + ::bigT) & 65535] = sample;
            ::samples[(i + ::bigT + 256) & 65535] = 0;
            ::samples[(i + ::bigT + 512) & 65535] = 128;
            ::samples[(i + ::bigT + 768) & 65535] = 255;
        }
        ::bigT += len;
        for (int i = 0; i < BUFFER_SIZE; i++)
            ::samples2[i] = audioBuffer[i];
    }
};

void update(SDL_Window *window, SDL_Renderer *renderer, long frame)
{
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    long millis = SDL_GetTicks64();
    audiovisual::drawVisualization(windowWidth, windowHeight, millis, renderer, frame);
    for (int y = 0; y < 256; y++)
    {
        const SDL_Rect *rect = new SDL_Rect{256, y, 256, 1};
        SDL_SetRenderDrawColor(renderer,
                               (int)(SDL_sin(millis / 1000.0 + (y / 256.0) + (PI * 0.0 / 3.0)) * 48 + 48)+32,
                               (int)(SDL_sin(millis / 1000.0 + (y / 256.0) + (PI * 2.0 / 3.0)) * 48 + 48)+32,
                               (int)(SDL_sin(millis / 1000.0 + (y / 256.0) + (PI * 4.0 / 3.0)) * 48 + 48)+32, 255);
        SDL_RenderFillRect(renderer, rect);
        delete rect;
    }
    int charIdx = 0;
    char row = 0;
    char col = 0;
    while (::input[charIdx] != 0)
    {
        if (::input[charIdx] == '\n')
        {
            if (cursorPos == charIdx)
            {
                audiovisual::drawFont(renderer, 0, (col + 32) * 8, row * 8, colors::white, 1);
            }
            col = 0;
            row++;
            charIdx++;
        }
        else
        {
            audiovisual::drawFont(renderer, chartoIdx(::input[charIdx]), (col + 32) * 8, row * 8, colors::white, cursorPos == charIdx);
            ++charIdx;
            ++col;
            if (col == 32)
            {
                col = 0;
                ++row;
            }
        }
    }
    if (cursorPos == charIdx)
    {
        audiovisual::drawFont(renderer, 0, (col + 32) * 8, row * 8, colors::white, 1);
    }
    charIdx = 0;
    row = 0;
    col = 0;
    while (::errorText[charIdx] != 0)
    {
        if (::errorText[charIdx] == '\n')
        {
            col = 0;
            row++;
            charIdx++;
        }
        else
        {
            audiovisual::drawFont(renderer, chartoIdx(::errorText[charIdx++]), (col + 32) * 8, (29 + row) * 8, colors::red, false);
            col++;
            if (col == 32)
            {
                col = 0;
                row++;
            }
        }
    }
    audiovisual::drawFont(renderer, chartoIdx('t'), 256, 248, colors::green, false);
    audiovisual::drawFont(renderer, chartoIdx('='), 264, 248, colors::green, false);
    std::string tstring = std::to_string(bigT);
    for (int i = 0; i < tstring.length(); i++)
    {
        audiovisual::drawFont(renderer, chartoIdx(tstring[i]), 256+((i+2)*8), 248, colors::green, false);
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
                if(keycode == 1073742051) continue;
                if (::CTRLKEY)
                {
                    if (keycode == 'w')
                    {
                        drawSettings::waveform = !drawSettings::waveform;
                        SDL_SetWindowSize(window, 512, drawSettings::waveform ? 512 : 256);
                    }
                    else if (keycode == 's')
                    {
                        if (::input[0] == '#' && ::input[1] == '#')
                        {
                            int endIndex = 2;
                            int startIndex = 2;
                            while (::input[endIndex] != '\n' && ::input[endIndex] != '\0')
                                endIndex++;
                            startIndex = endIndex;
                            while (::input[startIndex] == '\n' || ::input[startIndex] == ' ')
                                startIndex++;
                            if (::input[endIndex] == '\0' || ::input[startIndex] == '\0')
                                continue;
                            char *savePath = new char[256];
                            char *saveCode = new char[1280];
                            for (int i = 0; i < endIndex - 2; i++)
                            {
                                savePath[i] = ::input[i + 2];
                            }
                            savePath[endIndex - 2] = '\0'; // Null terminator

                            int codeSize = 0;

                            {
                                int i=2;
                                int j=0;
                                // Shebang
                                saveCode[0] = '#';
                                saveCode[1] = '!';
                                // Exec path
                                for(j = 0; ::argv0[j] != '\0'; j++) {
                                    saveCode[i++] = ::argv0[j];
                                }
                                saveCode[i++] = '\n';
                                saveCode[i++] = '\n';
                                // Code
                                for(j = startIndex; ::input[j] != '\0'; j++) {
                                    saveCode[i++] = ::input[j];
                                }
                                saveCode[i++] = '\n';
                                saveCode[i] = '\0';
                                codeSize = i;
                            }

                            std::ofstream file(savePath);
                            std::cout << std::boolalpha << file.is_open() << " " << savePath << std::endl;
                            if (file.is_open())
                            {
                                file.write(saveCode, codeSize);
                                strcpy(::errorText, "");
                            }
                            else
                            {
                                strcpy(::errorText, "Couldn't save file");
                                if (file.fail())
                                {
                                    strcat(::errorText, ":\n");
                                    strcat(::errorText, std::strerror(errno));
                                }
                            }
                            delete[] savePath;
                            delete[] saveCode;
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
                    else if (keycode == SDLK_F4)
                    {
                        if (::bigT >= 32768)
                            ::bigT -= 32768;
                    }
                    else if (keycode == SDLK_F5)
                    {
                        ::bigT += 32768;
                    }
                    else if (keycode == SDLK_F6)
                    {
                        ::frame = ::bigT = 0;
                        deleteGlobals();
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

// We need this for converting from relative to absolute path
char* stringToChar(std::string str) {
    int size = str.size();
    char* out = new char[size+1];
    out[size] = '\0';
    for(int i = 0; i < size; i++) {
        out[i] = str[i];
    }
    return out;
}

int main(int argc, char **argv)
{
    if (argc > 0) {
        std::filesystem::path path(argv[0]);
        if(!path.is_absolute()) {
        // Convert argv[0] to an absolute path
        std::filesystem::path pwd = std::filesystem::current_path();
        std::filesystem::path absolutePath = pwd / path;

            ::argv0 = stringToChar(absolutePath.string());
        }
        else ::argv0 = argv[0];
    }
    else
    {
        ::argv0 = new char[2];
        ::argv0[0] = '.';
        ::argv0[10] = '\0';
    }
    std::cout << version << std::endl;
    try
    {
        std::cout << "Heap allocation..." << std::endl;
        ::input = new char[1024];
        ::errorText = new char[64];
        ::memory = new int[memory_size];
        ::samples = new unsigned char[65536];
        ::samples2 = new unsigned char[BUFFER_SIZE];
        // Windows: zero out memory
        std::cout << "Heap init..." << std::endl;

        for (int i = 0; i < 1024; i++)
        {
            ::input[i] = 0;
        }

        for (int i = 0; i < 64; i++)
        {
            ::errorText[i] = 0;
        }

        for (int i = 0; i < memory_size; i++)
        {
            ::memory[i] = 0;
        }

        for (int i = 0; i < 65536; i++)
        {
            ::samples[i] = 0;
        }

        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            ::samples2[i] = 0;
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
        SDL_Window *window = SDL_CreateWindow(version, 32, 32, 512, 512, SDL_WINDOW_SHOWN); //  | SDL_WINDOW_RESIZABLE
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
        std::cout << "Starting...\n";

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
                char* buffer = new char[1280];
                for( int i = 0; i < 1280; i++ ) buffer[i] = 0;
                file.read(buffer, 1280);
                int i = 0;
                int j = 0;
                if(buffer[0]=='#' && (buffer[1]=='#' || buffer[1]=='!')) {
                    // Shebang/path extraction
                    while(buffer[i]!='\n' && buffer[i]!='\0') {
                        i++;
                    }
                }
                while(buffer[i]=='\n' || buffer[i]==' ') {
                    i++;
                }
                // ##File/path
                ::input[j++] = '#';
                ::input[j++] = '#';
                for(int k=0; argv[1][k]!='\0'; k++) {
                    input[j++] = argv[1][k];
                }
                ::input[j++] = '\n';
                ::input[j++] = '\n';
                // Code
                while(j < 1024) {
                    ::input[j++] = buffer[i++];
                }
            }
            else
            {
                char *error = new char[1024];
                for (int i = 0; i < 1024; i++)
                    error[i] = 0;
                strcat(error, "# Couldn't open the file:\n# ");
                strcat(error, argv[1]);
                strcat(error, "\n\ntAr2A&*C0&\nFFt3rC0&-*8r");
                strcpy(::input, error);
                delete[] error;
            }
        }
        else
        {
            char *defaultInput = new char[1024];
            for (int i = 0; i < 1024; i++)
                defaultInput[i] = 0;
            strcat(defaultInput, "# ");
            strcat(defaultInput, version);
            strcat(defaultInput, "\n\ntAr2A&*C0&\nFFt3rC0&-*8r");
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

        std::cout << "Closing SDL...\n";
        SDL_CloseAudio();
        SDL_Quit();

        std::cout << "Freeing heap...\n";
        delete[] ::memory;
        delete[] ::samples;
        delete[] ::samples2;
        delete[] ::input;
        delete[] ::errorText;
        delete[] ::argv0;
    }
    catch (std::bad_alloc &e)
    {
        std::cerr << "Failed to allocate the memory heap. (" << e.what() << ")\n";
        return -1;
    }
    return 0;
}
