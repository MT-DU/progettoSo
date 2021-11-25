spaced: ./window/gestisciFinestre.h ./menu/menu.h ./errors/errors.h gestisciFinestre.o menu.o errors.o main.o
	gcc ./window/gestisciFinestre.h ./menu/menu.h ./errors/errors.h gestisciFinestre.o menu.o errors.o main.o -o spaced -lncurses

main.o: ./menu/menu.h ./errors/errors.h ./window/gestisciFinestre.h  main.c 
	gcc -c ./menu/menu.h ./errors/errors.h ./window/gestisciFinestre.h  main.c

menu.o: ./errors/errors.h ./menu/menu.h ./menu/menu.c ./window/gestisciFinestre.h ./window/gestisciFinestre.c
	gcc -c ./errors/errors.h ./menu/menu.h ./menu/menu.c ./window/gestisciFinestre.h ./window/gestisciFinestre.c

gestisciFinestre.o: ./window/gestisciFinestre.h ./window/gestisciFinestre.c
	gcc -c ./window/gestisciFinestre.h ./window/gestisciFinestre.c

errors.o: ./errors/errors.h ./errors/errors.c
	gcc -c ./errors/errors.h ./errors/errors.c

clean:
	rm -f *.o
	rm -f ./*/*.o
	rm -f ./*/*.gch
	rm -f spaced