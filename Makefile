all : main.cpp
	g++ main.cpp chip8.cpp -lsfml-graphics -lsfml-window -lsfml-system -o output
