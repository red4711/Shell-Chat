OBJECTS = server client
OBJECT_FILE = server.o client.o chatroom_serialization.o
DEPS=chatroom_serialization.h

CC=gcc
CFLAGS =-ggdb -Wall


all: $(OBJECTS)


server: server.o chatroom_serialization.o
	$(CC) $(CFLAGS) $^ -o $@

client: client.o chatroom_serialization.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c $(DEPS)
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f $(OBJECT_FILE)
