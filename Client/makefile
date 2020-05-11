CC=gcc
CFLAGS= -Wall -pedantic -g
LIBS= -lSDL2 -lSDL2_ttf -lpthread
SOURCES = client_library.c UI_library.c client.c
OBJECTS = client_library.o UI_library.o client.o


client: $(OBJECTS)
		gcc $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)
		
client.o = client.c
UI_library.o = UI_library.h UI_library.c
client_library.o = client_library.h client_library.c

clean::
		rm -f *.o core a.out client *~
		
depende::
		makedepend $(SOURCES)

	
