all:
	g++ main.cpp Chip8.cpp Beep.cpp -o Chip8 -lSDL2
beep:
	g++ Beep.cpp -o beep -lSDL2
clean:
	rm main beep
