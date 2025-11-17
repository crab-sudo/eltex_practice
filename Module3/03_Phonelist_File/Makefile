COMPIL   = gcc
FLAGS    = -g -c -Wall


TARGET   = phonelist.out
SOURCES  = main.c backend.c interface.c
OBJECTS  = $(SOURCES:.c=.o)



all: $(TARGET)


$(TARGET): $(OBJECTS) $(LIB_NAME)
	$(COMPIL) $(OBJECTS) -o $@


%.o: %.c
	$(COMPIL) $(FLAGS) $< -o $@


run: $(TARGET)
	./$(TARGET)


clean:
	rm -f *.o $(TARGET)
