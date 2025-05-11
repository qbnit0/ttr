timetrack: main.c ansi.h tasks.c tasks.h timefmt.c timefmt.h
	$(CC) -Os main.c tasks.c timefmt.c -o timetrack
