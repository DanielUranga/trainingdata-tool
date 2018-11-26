MODULE_SRC= example_3.cpp
MODULE_OBJ= example_3.o
MODULE_TARGET= example_3
MODULE_INCS= -I../include/
CCFLAGS= -ansi -pedantic -Wall -g -c
LDFLAGS= -g -o

$(MODULE_TARGET): $(MODULE_OBJ)
	g++ $(MODULE_OBJ) -lpgn $(LDFLAGS) ./$(MODULE_TARGET)

.cpp.o:
	g++ $(CCFLAGS) $< $(MODULE_INCS)

clean:
	rm -f *.o
	rm -f ./$(MODULE_TARGET)

