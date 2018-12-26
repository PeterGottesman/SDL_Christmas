EXE=periodic
OBJS=src/main.c\
	 src/window.c\
	 src/game.c

CC=clang
FLAGS=-lSDL2 -lm -g

all: $(OBJS)
	$(CC) $(OBJS) $(FLAGS) -o $(EXE)

windows: $(OBJS)
	$(WINCC) $(OBJS) $(WINFLAGS) -o $(EXE).exe

clean:
	rm -f $(EXE)
