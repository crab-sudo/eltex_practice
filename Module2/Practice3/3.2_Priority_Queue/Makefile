COMPIL=gcc
FLAGS= -g -c -Wall
TARGET=priority_queue.out
SOURSES=main.c backend.c gui.c
OBJECTS=$(SOURSES:.c=.o)

all: $(SOURSES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(COMPIL) $(OBJECTS) -o $@ -lm

.cpp.o:
	$(COMPIL) $(FLAGS) $< -o $@