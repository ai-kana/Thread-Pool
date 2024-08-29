CC := cc
CFLAGS := -Wall -Wextra -Ofast

pool: main.o
	$(CC) $(CFLAGS) -g -o $@ $<

main.o: main.c
	$(CC) $(CFLAGS) -g -c -o $@ $<

.PHONY: clean
clean:
	rm pool
	rm main.o
