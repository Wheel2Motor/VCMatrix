CC:=gcc -std=c23
CFLAGS:=-O3 -DNDEBUG -Wextra -ftree-vectorize

all: vcmatrix.dll

vcmatrix.o: vcmatrix.c vcmatrix.h
	gcc -fPIC -c vcmatrix.c

vcmatrix.dll: vcmatrix.o
	gcc -shared -o $@ $^ $(CFLAGS)

example/test.exe: example/test.c vcmatrix.o
	gcc -o $@ $^ $(CFLAGS) -I.

testc: example/test.exe
	example/test.exe

testpy: vcmatrix.dll
	python example/test.py

clean:
	-rm *.o
	-rm *.dll
	-rm *.exe

.PHONY: all clean testc testpy
