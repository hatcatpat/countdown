all: countdown

countdown: countdown.c
	gcc -g -o countdown countdown.c -lm

run: all
	./countdown

clean:
	rm countdown
