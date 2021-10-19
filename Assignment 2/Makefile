CC=gcc
TARGET=takeaway
SOURCES=takeaway.c
OBJECTS=${SOURCES:.c=.o}
DEPS=
CFLAGS=-Wall -O3
LFLAGS=-lpthread
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
$(TARGET): $(OBJECTS)
	@echo " Linkning..."
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)
.PHONY: clean
clean:                                                                          
	@echo " Cleaning..."
	$(RM) -r *.o $(TARGET)
