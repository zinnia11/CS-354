###############################################################################
# @file: Makefile
#
# Simple makefile for project maintanence, usage, and testing 
#
# @creator: 11/18/21 Bujji Setty (selagamsetty@wisc.edu)
# @modified: 6/26/22 Bujji Setty (selagamsetty@wisc.edu)
#
###############################################################################

all: build_ll build_ht build_stress

build_ll: src/ll_functions.c drivers/ll_main.c
	@gcc -g -Iinc/ -o ll.out src/*.c drivers/ll_main.c -m64 -Wall -Werror=implicit-function-declaration

build_ht: src/hash_table.c drivers/ht_main.c src/ll_functions.c
	@gcc -g -Iinc/ -o ht.out src/*.c drivers/ht_main.c -m64 -Wall -Werror=implicit-function-declaration

build_stress: src/hash_table.c drivers/stress.c src/ll_functions.c
	@gcc -g -Iinc/  -o stress.out src/*.c drivers/stress.c -m64 -Wall -Werror=implicit-function-declaration

valgrind_ll: ll.out
	@valgrind -s --leak-check=full ./ll.out

valgrind_ht: ht.out
	@valgrind -s --leak-check=full ./ht.out

valgrind_stress: stress.out
	@valgrind -s --leak-check=full ./stress.out
    
clean: 
	@rm -f *.out

# Feel free to add any additional commands that may make testing easier. Our 
# grading scripts will be using the version of this file that was provided to 
# you when the project was released. 

