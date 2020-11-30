librcmalloc.so: rcmalloc.o
	gcc -shared -o librcmalloc.so rcmalloc.o

rcmalloc.o: rcmalloc.c
	gcc -c -fpic rcmalloc.c -Wall -Werror

clean:
	rm rcmalloc.o rcmalloc librcmalloc.so
