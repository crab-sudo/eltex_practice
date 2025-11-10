COMPIL=gcc
FLAGS= -g -c -Wall
TARGET=phonelist
SOURSES=main.c backend.c interface.c
OBJECTS=$(SOURSES:.c=.o)

all: $(SOURSES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(COMPIL) $(OBJECTS) -o $@

.cpp.o:
	$(COMPIL) $(FLAGS) $< -o $@