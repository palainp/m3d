OPT=-g -O0
#OPT=-Ofast
#OPT=-O3 -DNDEBUG

# gnu11 au lieu de c11 permet d'avoir (entre autre) la definition de M_PI
OPT_NORME=-Wall -Werror -Wextra -Wconversion -ansi -Wpedantic -Wno-unused-parameter -std=gnu11

###################
prog: prog.o lib_surface.o lib_2d.o lib_3d.o lib_mat.o lib_objet3d.o
	gcc ${OPT} ${OPT_NORME} -march=native $^ -o $@ -lm -lSDL2

run: prog
	./prog

%.o: %.c %.h
	gcc ${OPT} ${OPT_NORME} -c -march=native -o $@ $<

prog.o: prog.c
	gcc ${OPT} ${OPT_NORME} -c -march=native -o $@ $<

###################
clean:
	rm -f *.o prog *~
