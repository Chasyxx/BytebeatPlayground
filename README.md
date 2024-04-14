# Bytebeat playground
Inspired by things such as the [Ideally Bare Numeric Impression giZmo](http://viznut.fi/ibniz/) and classial C snippets of bytebeat music (in short, C/JS expressions that generate raw sample data), Bytebeat Playground is a runtime written in C++ for bytebeat-like music. Most C-compatible bytebeat codes (or at least their sounds) can be fully recreated in this engine.

## Compilation
### Prerequisites
* [SDL 2.0](https://www.libsdl.org/)
* GNU Make
* Some C++ compiler such as g++
* Perl (It's for the font!)
### Instructions
1. Ensure you're in the source tree
2. `./configure.sh`
3. `make clean`
4. `make`

After this, you should have a `BytebeatPlayground` program you can run.

Bytebeat Playground is under the GNU GPLv3, and you can distribute unmodified source code without worry. See the COPYING file or <https://www.gnu.org/licenses/gpl.html>. *This program comes with no warranty, and you are liable for damages.*

Copyright © 2023, 2024 Chase Taylor <creset200@gmail.com>

![Logo for the GNU GPLv3](https://www.gnu.org/graphics/gplv3-with-text-136x68.png)
