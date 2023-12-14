# Bytebeat playground
Inspired by things such as the [Ideally Bare Numeric Impression giZmo](http://viznut.fi/ibniz/) and classial C snippets of bytebeat music (in short, C/JS expressions that generate raw sample data), Bytebeat Playground is a runtime written in C++ for bytebeat music. Most C-compatible bytebeat codes' sounds can be fully recreated in this engine (the song that initially plays when the program starts is from http://www.pouet.net/topic.php?which=8357&page=6#c388670).

## Building
Prerequisites:
* SDL 2.0
* CMake 3.27 or higher
* Makefile
* Some C++ compiler, preferrably g++
```sh
    cmake .
    make
```
or run build.sh (which will also automatically open Bytebeat Playground)
