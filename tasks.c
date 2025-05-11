#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "tasks.h"

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
