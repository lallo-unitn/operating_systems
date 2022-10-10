default: all

all: msgDispatcher.out

msgDispatcher.out: 07-msgDispatcher.c
	@gcc -pthread -o msgDispatcher.out 07-msgDispatcher.c

clean:
	mkdir -p build
	rm -rf build *.out *.s

.PHONY: clean

.SECONDARY: make.s
