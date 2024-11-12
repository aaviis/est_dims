CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99
LDFLAGS=

SRCS=functions_gp.c functions_boxc.c main.c 
OBJS=$(SRCS:.c=.o)
HDRS=functions_gp.h functions_boxc.h main.h
TARGET=est_corrint

.PHONY: all clean

all: clean $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del -f $(OBJS) $(TARGET)
