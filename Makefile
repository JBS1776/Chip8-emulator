all:
	g++ main.cpp Chip8.cpp Beep.cpp -o main -I include -L lib -l SDL2-2.0.0
beep:
	g++ Beep.cpp -o beep -I include -L lib -l SDL2-2.0.0
clean:
	rm main beep
