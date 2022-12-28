OBJECTS = main.o DoCountPage.o PrintCountPage.o
SRC = main.c DoCountPage.c PrintCountPage.c

cc = gcc
CFLAGS = -O

TARGET = STARWARS
$(TARGET): $(OBJECTS) 
	$(CC) -o $(TARGET) $(OBJECTS)

main.o : Header.h main.c
DoCountPage.o : Header.h DoCountPage.c
PrintCountPage.o : Header.h PrintCountPage.c

clean:
	rm $(OBJECTS) $(TARGET)