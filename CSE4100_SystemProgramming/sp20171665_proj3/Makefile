OBJECTS = 20171665.o AssemblePassOne.o AssemblePassTwo.o LoadPassOne.o LoadPassTwo.o RunProgram.o
SRC = 20171665.c

cc = gcc
CFLAGS = -O

TARGET = 20171665.out
$(TARGET): $(OBJECTS) 
	$(CC) -W -Wall -o $(TARGET) $(OBJECTS) -lm
	rm -f $(OBJECTS)

20171665.o : 20171665.h 20171665.c
	gcc -W -Wall -c 20171665.c
AssemblePassOne.o : 20171665.h AssemblePassOne.c
	gcc -W -Wall -c AssemblePassOne.c
AssemblePassTwo.o : 20171665.h AssemblePassTwo.c
	gcc -W -Wall -c AssemblePassTwo.c
LoadPassOne.o : 20171665.h LoadPassOne.c
	gcc -W -Wall -c LoadPassOne.c
LoadPassTwo.o : 20171665.h LoadPassTwo.c
	gcc -W -Wall -c LoadPassTwo.c
RunProgram.o : 20171665.h RunProgram.c
	gcc -W -Wall -c RunProgram.c

clean:
	rm -f *.o
	rm -f $(TARGET)
