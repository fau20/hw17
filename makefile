all: control.c writing.c
	gcc -o control control.c
	gcc -o writing writing.c
