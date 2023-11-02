OPT=-g -O0
#OPT=-Ofast
#OPT=-O3 -DNDEBUG

# gnu11 au lieu de c11 permet d'avoir (entre autre) la definition de M_PI
OPT_NORME=-Wall -Werror -Wextra -Wconversion -ansi -Wpedantic -Wno-unused-parameter -Wno-unused-function -Wno-stringop-truncation -std=gnu11

###################
prog: prog.o lib_surface.o lib_2d.o lib_3d.o lib_mat.o lib_objet3d.o lib_scene3d.o chargement_scene3d.o
	gcc ${OPT} ${OPT_NORME} -march=native $^ -o $@ -lm -lSDL2

run: prog scene.scn
	./prog ./scene.scn

%.o: %.c %.h
	gcc ${OPT} ${OPT_NORME} -c -march=native -o $@ $<

parser_scene3d.tab.c: parser_scene3d.y globals.h
	bison $< -o $@

chargement_scene3d.o: lexer_scene3d.l globals.h parser_scene3d.tab.c
	flex -l $<
	gcc ${OPT} ${OPT_NORME} -c -march=native -o $@ lex.yy.c

prog.o: prog.c
	gcc ${OPT} ${OPT_NORME} -c -march=native -o $@ $<

###################
clean:
	rm -f parser_scene3d.tab.c lex.yy.c *.o prog *~
