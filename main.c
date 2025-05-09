#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include "ansi.h"

int stoplasttask(FILE *trackfile) {
  fseek(trackfile, -1, SEEK_END);
  int lc = fgetc(trackfile);

  switch (lc) {
    case -1:
      return 2;
    case '\t':
      fprintf(trackfile, "%ld\n", time(0));
      return 0; // 0 -> task stopped
    case '\n':
      return 2; // 2 -> no task to stop
    default:
      return 1; // 1 -> other error
  }
}

struct task {
  char *name;
  long time;
};

struct task gettask(FILE *trackfile) {
  char *buf = malloc(1024);
  struct task notask = { NULL, 0 };

  fgets(buf, 1024, trackfile);

  char *times = buf;
  buf = strsep(&times, "\t");
  if (times == NULL) { return notask; }

  char *timestart = strsep(&times, "\t");
  if (timestart == NULL) { return notask; }
  long begintime = atol(timestart);

  char *timestop = strsep(&times, "\t \n");
  if (timestop == NULL) { return notask; }
  long endtime = atol(timestop);

  char *taskname = realloc(buf, strlen(buf) + 1);

  struct task thistask = { taskname, endtime - begintime };

  return thistask;
}

char *timefmt(long seconds) {
  char *ret = malloc(32);
  if (seconds < 60) { // Minute
    sprintf(ret, "%lds", seconds);
  } else if (seconds < 3600) { // Hour
    sprintf(ret, "%ldm%lds", seconds / 60, seconds);
  } else if (seconds < 86400) { // Day
    sprintf(ret, "%ldh%ldm", seconds / 3600, seconds / 60);
  } else if (seconds < 604800) { // Week
    sprintf(ret, "%ldd%ldh", seconds / 86400, seconds / 3600);
  } else {
    sprintf(ret, "%ldw%ldd", seconds / 604800, seconds / 86400);
  }
  return ret;
}

enum opmode {BEGINTRACK, ENDTRACK, SHOWTIMES};

int main (int argc, char** argv) {
  FILE *trackfile = NULL;
  char *tracktask = NULL;
  enum opmode opmode = SHOWTIMES;
  bool ok = true;

  int opt;
  while ((opt = getopt(argc, argv, "f:t:e")) != -1) {
    switch (opt) {
      case 'f':
        trackfile = fopen(optarg, "a+");
        break;
      case 't':
        tracktask = optarg;
        opmode = BEGINTRACK;
        break;
      case 'e':
        opmode = ENDTRACK;
        break;
    }
  }

  if (trackfile == NULL) {
    trackfile = fopen("tt.dat", "a+");
  }

  switch (opmode) {
    case BEGINTRACK:
      switch (stoplasttask(trackfile)) {
        case 0:
          printf(BLUE "Previous task stopped.\n");
          break;
        case 2:
          break;
        case 1:
          fprintf(stderr, RED "Unknown error, file may be invalid.\n");
          ok = false;
          break;
      }
      if (ok) {
        fprintf(trackfile, "%s\t%ld\t", tracktask, time(0));
        printf(GREEN "Task " RESET EMPHASIZE "%s" NORMAL GREEN " started.\n", tracktask);
      }
      break;
    case ENDTRACK:
      switch (stoplasttask(trackfile)) {
        case 0:
          printf(GREEN "Task " RESET "stopped" GREEN ".\n");
          break;
        case 2:
          printf(YELLOW "No task to stop.\n");
          break;
        case 1:
          fprintf(stderr, RED "Unknown error, file may be invalid.\n");
          ok = false;
          break;
      }
      break;
    case SHOWTIMES:
      rewind(trackfile);
      struct task task = {"", 0};

      char **names = calloc(1024, sizeof(char *));
      long *times = calloc(1024, sizeof(long));
      size_t taskcount = 0;
      size_t duptasks = 0;

      while (1) {
        task = gettask(trackfile);
        if (task.name == NULL) { break; }

        // Find duplicates
        size_t taskno = taskcount++;;
        for (size_t i = 0; i < taskno; i++) {
          if (names[i] != NULL && strcmp(names[i], task.name) == 0) {
            taskno = i;
            free(names[i]);
            names[i] = NULL;
            duptasks++;
          }
        }

        names[taskno] = task.name;
        times[taskno] += task.time;
      }

      printf(UNDERLINE "%ld tasks tracked\n" RESET, taskcount - duptasks);

      for (size_t i = 0; i < taskcount; i++) {
        if (names[i] == NULL) { continue; }
        char *name = names[i];
        char *time = timefmt(times[i]);

        printf(EMPHASIZE "%s" RESET YELLOW "\t\t%s\n" RESET, name, time);

        free(name);
        free(time);
      }

      break;
  }

  fclose(trackfile);

  return ok ? 0 : 1;
}
