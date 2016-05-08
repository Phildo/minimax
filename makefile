
run: main.c
	gcc -O3 main.c -o a.out && ./a.out

debug:
	gcc -ggdb3 -O0 main.c -o a.out && gdb -q ./a.out

valgrind:
	gcc -g -O0 main.c -o a.out && valgrind -v --leak-check=yes --leak-check=full --show-leak-kinds=all ./a.out

