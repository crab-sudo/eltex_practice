COMPIL   = gcc
FLAGS    = -g -c -Wall


TARGET   = perent-child.out
SOURCES  = main.c
OBJECTS  = $(SOURCES:.c=.o)



all: $(TARGET)


$(TARGET): $(OBJECTS) $(LIB_NAME)
	$(COMPIL) $(OBJECTS) -o $@


%.o: %.c
	$(COMPIL) $(FLAGS) $< -o $@


clean:
	rm -f *.o $(LIB_NAME) $(TARGET)
