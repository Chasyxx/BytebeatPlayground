# Bytebeat playground
Inspired by things such as the [Ideally Bare Numeric Impression giZmo](http://viznut.fi/ibniz/) and classial C snippets of bytebeat music (in short, C/JS expressions that generate raw sample data), Bytebeat Playground is a runtime written in C++ for bytebeat music. Most C-compatible bytebeat codes' sounds can be fully recreated in this engine.

## Compilation
### Prerequisites
* [SDL 2.0](https://www.libsdl.org/), for audio and rendering
* CMake 3.27 or higher
* Makefile
* Some C++ compiler such as g++
### Instructions
1. Use CMake to create the makefile: `cmake .`
2. Use Make to generate the binary: `make`

After this, you should have a `BytebeatPlayground` or `BytebeatPlayground.exe` binary. `build.sh` is a Bash script for GNU/Linux that performs these instructions automatically.

Bytebeat Playground is under the GNU GPLv3, and can be freely distributed and modified under certain conditions. See the COPYING file or <https://www.gnu.org/licenses/gpl.html>. This program comes with ABSOLUTELY NO WARRANTY.
Copyright © 2023, 2024 Chase T. <creset200@gmail.com>

![GPL3 logo](https://www.gnu.org/graphics/gplv3-with-text-136x68.png)
