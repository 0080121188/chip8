all : main.cpp
	g++ main.cpp chip8.cpp -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -Ofast -o chip8_emulator
