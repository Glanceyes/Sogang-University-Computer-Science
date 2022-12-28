OBJECTS = main.o String_Manipulation.o Output.o
SRC = main.cpp String_Manipulation.cpp Output.cpp

cc = g++
CFLAGS = -o

TARGET = main
$(TARGET): $(OBJECTS) Header.h
	$(CC) -o $(TARGET) $(OBJECTS)

clean:
	rm $(OBJECTS) $(TARGET)
