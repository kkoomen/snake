EXEC = snake.out
SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:.c=.o)
CFLAGS = -Wall -Wextra -pedantic
LDFLAGS = -lncurses

$(EXEC): $(OBJECTS)
	gcc $(OBJECTS) $(CFLAGS) -o $(EXEC) $(LDFLAGS)

%.o: %.c include/%.h
	gcc -c $(CFLAGS) $< -o $@

clean:
	-rm *.out
	-rm*.o
	-rm src/*.o

install:
	cp snake.out /usr/local/bin/snake

uninstall:
	rm /usr/local/bin/snake
