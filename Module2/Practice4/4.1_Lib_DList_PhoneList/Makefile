COMPIL=gcc
FLAGS= -g -c -Wall

LIB_NAME=libphonelist.a
LIB_OBJ=backend.o
LIB_SOURCE=backend.c

TARGET=phonelist.out
SOURSES=main.c interface.c
OBJECTS=$(SOURSES:.c=.o)

all: $(SOURSES) $(TARGET)

$(LIB_NAME): $(LIB_OBJ)
	ar rcs $@ $^

$(LIB_OBJ): $(LIB_SOURCE)
	$(COMPIL) $(FLAGS) $< -o $@

$(TARGET): $(OBJECTS) $(LIB_NAME)
	$(COMPIL) $(OBJECTS) -L. -lphonelist -o $@

.cpp.o:
	$(COMPIL) $(FLAGS) $< -o $@

clean:
	rm -f *.o $(LIB_NAME) $(TARGET)