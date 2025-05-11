#include <stdlib.h>
#include <stdio.h>

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
