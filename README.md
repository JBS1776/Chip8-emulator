# Chip8-emulator

Repo no longer includes the files for SDL2.  I edited the Makefile and added the flags for SDL2 to be included in the compilation.
<br>
You must install SDL2 on your computer before being able to build the program.  On MAC, use brew to install SDL2 and on Linux use apt, yum, etc. to install it.
<br>
You should be able to rebuild the application running make after installation of SDL2
<br>
In the near future, I plan to build a .sln file from Visual Studio so this can be run through Windows
<br>
I also plan on adding config menus to be able to configure user input
<br>

This is an emulator for the Chip 8 interpreter.  This was a fun side project I completed.
Since this is my first emulation, I referred to the below two sources for help with the implementation.
<br>
I used Mac OSX with only g++ for compilation so the original commit includes the SDL.h files
https://austinmorlan.com/posts/chip8_emulator/
<br>
https://github.com/JamesGriffin/CHIP-8-Emulator
