OBJECTS = 20171665.o
SRC = 20171665.c

cc = gcc
CFLAGS = -O

TARGET = 20171665.out
$(TARGET): $(OBJECTS) 
	$(CC) -W -o $(TARGET) $(OBJECTS)
	rm -f 20171665.o

20171665.o : 20171665.h 20171665.c

clean:
	rm -f *.o
	rm -f $(TARGET)
