CFLAGS=-Wall -Wextra -g -lm
EXE=bmp

build: $(EXE)

$(EXE): $(EXE).c
	gcc $(CFLAGS) $^ -o $@

run:
	./$(EXE)

clean:
	rm -f $(EXE)
