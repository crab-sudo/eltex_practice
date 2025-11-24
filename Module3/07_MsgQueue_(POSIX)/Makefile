COMPIL   = gcc
FLAGS    = -g -c -Wall

TARGET   = server.out
SOURCES  = main.c backend.c
OBJECTS  = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(COMPIL) $(OBJECTS) -o $@ -lrt

%.o: %.c
	$(COMPIL) $(FLAGS) $< -o $@

clean:
	rm -f *.o server.out client.out
