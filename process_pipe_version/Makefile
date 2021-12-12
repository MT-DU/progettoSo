spaced: ./window/gestisciFinestre.h ./menu/menu.h ./game/game.h ./errors/errors.h gestisciFinestre.o game.o menu.o errors.o main.o
	gcc ./window/gestisciFinestre.h ./menu/menu.h ./game/game.h ./errors/errors.h gestisciFinestre.o game.o menu.o errors.o main.o -o spaced -lncurses

main.o: ./menu/menu.h main.c 
	gcc -c ./menu/menu.h main.c

menu.o:  ./menu/menu.h ./game/game.h ./menu/menu.c 
	gcc -c ./menu/menu.h ./game/game.h ./menu/menu.c

game.o: ./game/game.h ./errors/errors.h ./window/gestisciFinestre.h ./game/game.c
	gcc -c ./game/game.h ./errors/errors.h ./window/gestisciFinestre.h ./game/game.c

gestisciFinestre.o: ./window/gestisciFinestre.h ./window/gestisciFinestre.c
	gcc -c ./window/gestisciFinestre.h ./window/gestisciFinestre.c

errors.o: ./errors/errors.h ./errors/errors.c
	gcc -c ./errors/errors.h ./errors/errors.c

clean:
	rm -f *.o
	rm -f ./*/*.o
	rm -f ./*/*.gch
	rm -f spaced
	rm *.txt