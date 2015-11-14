EXE=.exe

SRCS=\
	bggl3_types.c \
	bggl3_parse.c \
	bggl3_interp.c \
	bggl3_builtin.c \
	bggl3_builtin_graph.c \
	bggl3_builtin_vector.c \
	bggl3_builtin_opr.c \
	bggl3.c

OBJS=$(SRCS:.c=.o)

CFLAGS=-g

all: bggl3$(EXE)

autohead$(EXE): autohead.c
	gcc -o autohead$(EXE) autohead.c

bggl3_auto.h: autohead$(EXE) $(SRCS)
	./autohead$(EXE) $(SRCS) > bggl3_auto.h

bggl3$(EXE): bggl3_auto.h $(OBJS)
	gcc -g -o bggl3$(EXE) $(OBJS) -lreadline

clean:
	rm -rf *.o