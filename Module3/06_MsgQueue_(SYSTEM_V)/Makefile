COMPIL   = gcc
FLAGS    = -g -c -Wall

S_TARGET   = server.out
S_SOURCES  = server.c server_backend.c
S_OBJECTS  = $(S_SOURCES:.c=.o)

C_TARGET   = client.out
C_SOURCES  = client.c client_backend.c
C_OBJECTS  = $(C_SOURCES:.c=.o)

all: $(S_TARGET) $(C_TARGET)

$(S_TARGET): $(S_OBJECTS)
	$(COMPIL) $(S_OBJECTS) -o $@

$(C_TARGET): $(C_OBJECTS)
	$(COMPIL) $(C_OBJECTS) -o $@

%.o: %.c
	$(COMPIL) $(FLAGS) $< -o $@

clean:
	rm -f *.o server.out client.out
