default: all

all: main.c

app.out: main.c
	@gcc -o app.out main.c

clean:
	mkdir -p build
	rm -rf build *.out *.s

.PHONY: clean

.SECONDARY: make.s
