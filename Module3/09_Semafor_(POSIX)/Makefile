COMPIL   = gcc
FLAGS    = -g -c -Wall

P_TARGET   = producer.out
P_SOURCES  = producer.c
P_OBJECTS  = $(P_SOURCES:.c=.o)

C_TARGET   = consumer.out
C_SOURCES  = consumer.c
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
