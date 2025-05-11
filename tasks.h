#ifndef TASKS_H
#define TASKS_H
int stoplasttask(FILE *trackfile);

struct task {
  char *name;
  long time;
};

struct task gettask(FILE *trackfile);
#endif
