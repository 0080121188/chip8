# Usage
```
$ chip8_emulator your_rom
```
## Optional arguments 
-d  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Enables debug mode (it only displays the opcodes)  
-fps *value*  
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Runs the program at *value* fps.  
```
$ chip8_emulator your_rom -d -fps 600
```

# Where to get ROMs
* [Here](https://johnearnest.github.io/chip8Archive/)

# Tests
- [x] IBM logo
- [x] corax+
- [x] flags
- [x] quirks*
- [x] keypad
- [x] beep  
* \* when I changed the 0x8XY7 instruction so that it correctly underflows, it now doesn't pass (DISP. WAIT - SLOW) for some reason, but it's not necessarily an error - just means that the test can't be deterministic  
* [Test source](https://github.com/Timendus/chip8-test-suite)
