Currently building Bytebeat Playground only needs you to compile a single file,
making sure to link SDL:
```sh
    gcc -o myprogram myprogram.c `sdl2-config --cflags --libs`
```

However, infrastructure for CMake already exists for whenever that's needed. you can run
```sh
    cmake .
    make
```

Or run build.sh if so preferred (will also automatically open Bytebeat Playground).

Most of the commands are similar to the Ideally Bare Numberic expression gIZmo, see https://viznut.fi/ibniz