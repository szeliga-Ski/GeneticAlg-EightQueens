# Makefile for 'GA_EightQueens'
CC = zcc
RM = rm -f
DEPS = EightQueens.h EightQueens.c functions.h functions.c defines.h
OUT = EightQueens.bin
CFLAGS = +zx -vn -O2 -preserve
OBJS = EightQueens.o functions.o
LIBS = -lndos -lpreempt

all: $(OBJS)
#	$(CC) $(CFLAGS) -create-app -pragma-define=myzorg=28000 -o $(OUT) $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) -create-app -o $(OUT) $(OBJS) $(LIBS)

clean:
	$(RM) $(OUT) *.o *.tap *.def *.reloc
