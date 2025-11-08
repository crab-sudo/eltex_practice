COMPIL=gcc
FLAGS= -g -c -Wall
TARGET=tipachmod
SOURSES=main.c backend.c gui.c
OBJECTS=$(SOURSES:.c=.o)

all: $(SOURSES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(COMPIL) $(OBJECTS) -o $@

.cpp.o:
	$(COMPIL) $(FLAGS) $< -o $@