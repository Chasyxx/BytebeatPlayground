Currently building Bytebeat Playground only needs you to compile a single file,
making sure to link SDL:
```sh
    g++ -o exec main.cpp `sdl2-config --cflags --libs`
```

However, infrastructure for CMake already exists for whenever that's needed. you can run
```sh
    cmake .
    make
```

in the top directory, or run build.sh (which will also automatically open Bytebeat Playground).

Most of the commands are similar to the Ideally Bare Numberic expression gIZmo, see https://viznut.fi/ibniz
