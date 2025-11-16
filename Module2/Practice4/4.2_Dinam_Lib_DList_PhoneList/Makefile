CC      = gcc
CFLAGS  = -g -c -Wall -fPIC


LIB_NAME = libphonelist.so
LIB_OBJ  = backend.o
LIB_SRC  = backend.c


TARGET   = phonelist.out
SOURCES  = main.c interface.c
OBJECTS  = $(SOURCES:.c=.o)


all: $(TARGET)


$(TARGET): $(OBJECTS) $(LIB_NAME)
	$(CC) $(OBJECTS) -L. -lphonelist -o $@


%.o: %.c
	$(CC) $(CFLAGS) $< -o $@


$(LIB_NAME): $(LIB_OBJ)
	$(CC) -shared -o $@ $^



run: $(TARGET)
	LD_LIBRARY_PATH=. ./$(TARGET)

clean:
	rm -f *.o $(LIB_NAME) $(TARGET)
