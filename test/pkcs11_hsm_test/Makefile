CC = gcc
TARGET = libp11_readhsm

LDFLAGS = -lp11 -lcrypto -ldl -lpthread

OBJECTS = $(TARGET).o
SRCS = $(TARGET).c

$(TARGET) : $(OBJECTS)
	$(CC) $(SRCS) $(LDFLAGS) -o $(TARGET)

clean:
	rm -rf $(OBJECTS) $(TARGET) core
