PREFIX := /usr/local
SRC    := src/main.c

yapm: ${SRC}
	${CC} $? -o bin/$@

install: yapm
	cp bin/yapm ${PREFIX}/bin

uninstall: clean
	rm -f ${PREFIX}/bin

clean:
	rm -f bin/yapm

debug: ${SRC}
	@time { ${CC} $? -g -o bin/$@; echo; }
