PREFIX = /usr/local
CC ?= gcc

yapm: src/main.c
	${CC} $? -o bin/$@

install: yapm
	cp bin/yapm ${PREFIX}/bin

uninstall: clean
	rm -f ${PREFIX}/bin

clean:
	rm -f bin/yapm
