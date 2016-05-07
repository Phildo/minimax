
run: main.c
	gcc main.c -o a.out && ./a.out

debug:
	gcc -ggdb3 main.c -o a.out && gdb -q ./a.out

