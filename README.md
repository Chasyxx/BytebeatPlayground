# Bytebeat playground
Inspired by things such as the [Ideally Bare Numeric Impression giZmo](http://viznut.fi/ibniz/) and classial C snippets of bytebeat music (in short, C/JS expressions that generate raw sample data), Bytebeat Playground is a runtime written in C++ for bytebeat music. Most C-compatible bytebeat codes' sounds can be fully recreated in this engine.

## Compilation
### Prerequisites
* [SDL 2.0](https://www.libsdl.org/), for audio and rendering
* make
* Some C++ compiler such as g++
### Instrucetions
1. Ensure you're in the source tree
2. `./configure.sh`
3. `make clean`
4. `make`
Now you have a `./bytebeatPlayground`.

After this, you should have a `BytebeatPlayground` or `BytebeatPlayground.exe` binary. `build.sh` is a Bash script for GNU/Linux that performs these instructions automatically.

Bytebeat Playground is under the GNU GPLv3, and can be freely distributed and modified under certain conditions. See the COPYING file or <https://www.gnu.org/licenses/gpl.html>. This program comes with no warranty. You are liable for damages.
Copyright © 2023, 2024 Chase T. <creset200@gmail.com>

![Logo for the GNU GPLv3](https://www.gnu.org/graphics/gplv3-with-text-136x68.png)
