build:
	gcc -Wall -std=c99 -I ./include ./src/*.c -lSDL2 -lm -o ./bin/renderer

run:
	./bin/renderer

clean:
	rm renderer
