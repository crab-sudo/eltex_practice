COMPIL   = gcc
FLAGS    = -g -c -Wall –lrt

P_TARGET   = parent.out
P_SOURCES  = parent.c
P_OBJECTS  = $(P_SOURCES:.c=.o)

C_TARGET   = child.out
C_SOURCES  = child.c
C_OBJECTS  = $(C_SOURCES:.c=.o)

all: $(P_TARGET) $(C_TARGET)



$(P_TARGET): $(P_OBJECTS)
	$(COMPIL) $(P_OBJECTS) -o $@

$(C_TARGET): $(C_OBJECTS)
	$(COMPIL) $(C_OBJECTS) -o $@



%.o: %.c
	$(COMPIL) $(FLAGS) $< -o $@

clean:
	rm -f *.o $(P_TARGET) $(C_TARGET)
