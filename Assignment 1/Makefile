CC=gcc
TARGET=cShell
SOURCES=main.c cs345shell.c
OBJECTS=${SOURCES:.c=.o}
DEPS=cs345.h
CFLAGS=-Wall -std=gnu99
LFLAGS=-lreadline
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
$(TARGET): $(OBJECTS)
	@echo " Linkning..."
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)
.PHONY: clean
clean:                                                                          
	@echo " Cleaning..."
	$(RM) -r *.o $(TARGET)
