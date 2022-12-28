OBJECTS = 20171665.o AssemblePassOne.o AssemblePassTwo.o
SRC = 20171665.c

cc = gcc
CFLAGS = -O

TARGET = 20171665.out
$(TARGET): $(OBJECTS) 
	$(CC) -W -o $(TARGET) $(OBJECTS)
	rm -f $(OBJECTS)

20171665.o : 20171665.h 20171665.c
AssemblePassOne.o : 20171665.h AssemblePassOne.c
AssemblePassTwo.o : 20171665.h AssemblePassTwo.c

clean:
	rm -f *.o
	rm -f $(TARGET)
