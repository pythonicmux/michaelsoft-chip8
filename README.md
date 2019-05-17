# michaelsoft-chip8
One of many Chip8 emulators written in C++ on GitHub, based on the writeup at http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/

I'm doing this just as an intro so I can actually kind of understand how to write an emulator before I try something harder (NES probably)!

It's a pretty simple project: there's a chip-8 class, and the main file just takes in a ROM file input and loads up a chip-8 machine with the ROM file. If you look at the chip8.cpp file, its emulation cycle function is a simple fetch (look at where the program counter is and get the instruction), decode (a giant switch statement LOL depending on the instruction) and execute (the code inside each condition that modifies the state).

Before compiling and running, install SDL2! For Mac, it's just
```
brew install sdl2
```
If you get a linking error, you may have to use chown on some directories to get permissions for brew to install sdl2.

To start up to compile the project, run 
```
cmake .
make
```

To run the executable use
```
./michaelsoft-chip8 <ROM>
```

The chip-8 has a hex keyboard (16 keys), and those keys are mapped to the keyboard as such:
```
 CHIP-8          KEYBOARD
 1 2 3 C         1 2 3 4
 4 5 6 D         Q W E R
 7 8 9 E         A S D F
 A 0 B F         Z X C V
```
I haven't actually played all the ROMs out there yet, but from the ones I've messed around with, keys 123, QWE and ASD on the computer keyboard generally dictate movement for the player in the games. Have fun!
