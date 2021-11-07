spaced: ./hud/hud.h ./errors/errors.h hud.o errors.o main.o
	gcc ./hud/hud.h ./errors/errors.h hud.o errors.o main.o -o spaced -lncurses

main.o: ./hud/hud.h ./errors/errors.h main.c
	gcc -c ./hud/hud.h ./errors/errors.h main.c

hud.o: ./errors/errors.h ./hud/hud.h ./hud/hud.c
	gcc -c ./errors/errors.h ./hud/hud.h ./hud/hud.c

errors.o: ./errors/errors.h ./errors/errors.c
	gcc -c ./errors/errors.h ./errors/errors.c

clean:
	rm -f *.o
	rm -f ./*/*.o
	rm -f ./*/*.gch
	rm -f spaced