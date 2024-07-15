# Usage
```
$ chip8_emulator your_rom
```
* *Need to have the beep.ogg file in the same folder as the chip8\_emulator.*  
## Optional arguments 
-d  
* Enables debug mode (it only displays the opcodes) 
   
-fps *value*  
* Runs the program at *value* fps.  
```
$ chip8_emulator your_rom -d -fps 10
```

# Where to get ROMs
* [Here](https://johnearnest.github.io/chip8Archive/)

# Tests
- [x] IBM logo
- [x] corax+
- [x] flags
- [x] quirks
- [x] keypad
- [x] beep  
* [Test source](https://github.com/Timendus/chip8-test-suite)
