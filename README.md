# michaelsoft-chip8
One of many Chip8 emulators written in C++ on GitHub, based on the writeup at http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/

I'm doing this just as an intro so I can actually kind of understand how to write an emulator before I try something harder (NES probably)!

It's a pretty simple project: there's a chip-8 class, and the main file just takes in a ROM file input and loads up a chip-8 machine with the ROM file. If you look at the chip8.cpp file, its emulation cycle function is a simple fetch (look at where the program counter is and get the instruction), decode (a giant switch statement LOL depending on the instruction) and execute (the code inside each condition that modifies the state).

To start up to compile the project, run 
```
cmake .
make
```

It's not done yet, but when it is done, to run it use
```
./michaelsoft-chip8 <ROM>
```
