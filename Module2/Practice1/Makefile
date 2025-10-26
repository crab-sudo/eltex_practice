COMPIL=gсс
FLAGS=-c -Wall
TARGET=target
SOURSES=main.cpp
OBJECTS=$(SOURSES:.cpp=.o)

all: $(SOURSES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(COMPIL) $(OBJECTS) -o $@

.cpp.o:
	$(COMPIL) $(FLAGS) $< -o $@
