COMPIL   = gcc
FLAGS    = -g -c -Wall


TARGET   = terminal.out
SOURCES  = main.c backend.c
OBJECTS  = $(SOURCES:.c=.o)



all: $(TARGET)


$(TARGET): $(OBJECTS) $(LIB_NAME)
	$(COMPIL) $(OBJECTS) -o $@


%.o: %.c
	$(COMPIL) $(FLAGS) $< -o $@


clean:
	rm -f *.o $(TARGET)
