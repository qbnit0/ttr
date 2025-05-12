#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include "ansi.h"
#include "strings.h"
#include "tasks.h"
#include "timefmt.h"

enum opmode {BEGINTRACK, ENDTRACK, SHOWTIMES};

int main (int argc, char** argv) {
  FILE *trackfile = NULL;
  char *tracktask = NULL;
  enum opmode opmode = SHOWTIMES;
  bool ok = true;

  int opt;
  opterr = 0;
  while ((opt = getopt(argc, argv, "f:t:ehv")) != -1) {
    switch (opt) {
      case 'h':
        printf(HELPMSG, argv[0]);
        return 0;
      case 'v':
        printf(VERSMSG);
        return 0;
      case 'f':
        trackfile = fopen(optarg, "a+");
        break;
      case 't':
        tracktask = optarg;
        opmode = BEGINTRACK;
        break;
      case 'e':
        opmode = ENDTRACK;
        if (opmode == BEGINTRACK) {
          // Make -e -t <> behave identically to -t <>
          opmode = BEGINTRACK;
        }
        break;
      case ':':
        fprintf(stderr, RED "%s: option requires an argument -- '%c'\n", argv[0], optopt);
        fprintf(stderr, YELLOW "Try %s -h for usage help.\n", argv[0]);
        return 1;
      case '?':
        fprintf(stderr, RED "%s: invalid option -- '%c'\n", argv[0], optopt);
        fprintf(stderr, YELLOW "Try %s -h for usage help.\n", argv[0]);
        return 1;
    }
  }

  if (trackfile == NULL) {
    char *homedir = getenv("HOME");
    char fname[256];
    snprintf(fname, sizeof(fname), "%s/.ttr", homedir);
    trackfile = fopen(fname, "a+");
  }

  if (trackfile == NULL) {
    perror(RED "Failed to open file");
    return 1;
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

      // Length of longest task name
      size_t maxnamelen = 0;

      while (1) {
        task = gettask(trackfile);
        if (task.name == NULL) { break; }
        size_t thislen = strlen(task.name);
        if (thislen > maxnamelen) { maxnamelen = thislen; }

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

      if (taskcount - duptasks == 1) {
        printf(UNDERLINE "1 task tracked\n" RESET);
      } else {
        printf(UNDERLINE "%ld tasks tracked\n" RESET, taskcount - duptasks);
      }


      for (size_t i = 0; i < taskcount; i++) {
        if (names[i] == NULL) { continue; }
        char *name = names[i];
        char *time = timefmt(times[i]);

        int padding = maxnamelen - strlen(name);
        printf(EMPHASIZE "%s" RESET YELLOW "%*s\t%s\n" RESET, name, padding, "", time);

        free(name);
        free(time);
      }

      free(names);
      free(times);

      break;
  }

  fclose(trackfile);

  return ok ? 0 : 1;
}
