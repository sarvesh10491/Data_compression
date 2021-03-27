# the compiler: g++ for C++ program
CC = g++
# LIBS = -pthread -lm -lrt
CFLAGS  = -Wall
# DEPS = huffcom.h
TARGET = huffcom

all: $(TARGET)

$(TARGET): $(TARGET).cpp $(DEPS)
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp $(LIBS)

clean:
	$(RM) $(TARGET)