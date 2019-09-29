build:
	gcc -O0 -pthread -no-pie -m64 -g -c lib.c respond.c listener.c 
	nasm -f elf64 main.asm 
	gcc -O0 -pthread -no-pie -m64 -o main respond.o listener.o main.o lib.o 
