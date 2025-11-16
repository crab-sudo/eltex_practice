COMPIL   = gcc
FLAGS    = -g -c -Wall


LIB_NAME = libphonelist.a
LIB_OBJ  = backend.o
LIB_SRC  = backend.c


TARGET   = phonelist.out
SOURCES  = main.c interface.c
OBJECTS  = $(SOURCES:.c=.o)



all: $(TARGET)


$(TARGET): $(OBJECTS) $(LIB_NAME)
	$(COMPIL) $(OBJECTS) -L. -lphonelist -o $@


%.o: %.c
	$(COMPIL) $(FLAGS) $< -o $@


$(LIB_NAME): $(LIB_OBJ)
	ar rcs $@ $^



clean:
	rm -f *.o $(LIB_NAME) $(TARGET)
