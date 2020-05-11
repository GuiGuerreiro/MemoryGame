CC=gcc
CFLAGS= -Wall -pedantic -g
LIBS= -lSDL2 -lSDL2_ttf -lpthread
SOURCES = board_library.c UI_library.c server_library.c server.c
OBJECTS = board_library.o UI_library.o server_library.o server.o


server: $(OBJECTS)
		gcc $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)
		
board_library.o = board_library.h board_library.c
UI_library.o = UI_library.h UI_library.c
server_library.o = server_library.h server_library.c
server.o = server.c

clean::
		rm -f *.o core a.out client *~
		
depende::
		makedepend $(SOURCES)

	
