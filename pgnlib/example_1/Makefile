MODULE_SRC= example_1.cpp
MODULE_OBJ= example_1.o
MODULE_TARGET= example_1
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

