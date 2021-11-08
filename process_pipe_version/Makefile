spaced: ./menu/menu.h ./errors/errors.h menu.o errors.o main.o
	gcc ./menu/menu.h ./errors/errors.h menu.o errors.o main.o -o spaced -lncurses

main.o: ./menu/menu.h ./errors/errors.h main.c
	gcc -c ./menu/menu.h ./errors/errors.h main.c

menu.o: ./errors/errors.h ./menu/menu.h ./menu/menu.c
	gcc -c ./errors/errors.h ./menu/menu.h ./menu/menu.c

errors.o: ./errors/errors.h ./errors/errors.c
	gcc -c ./errors/errors.h ./errors/errors.c

clean:
	rm -f *.o
	rm -f ./*/*.o
	rm -f ./*/*.gch
	rm -f spaced