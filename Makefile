# Makefile
TARGET = hamming
CC = gcc

$(TARGET): $(TARGET).c
	$(CC) -o $(TARGET) main.c $(TARGET).c print.c test.c

clean:
	rm -f $(TARGET)