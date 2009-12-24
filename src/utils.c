#include <stdio.h>
#include <signal.h>

#include "utils.h"

void log_message(char *message) {
  printf("LOG: %s\n", message);
}

int read_pid(char *path) {
  int pid = 0;
  FILE *f = fopen(path, "r");
  if(!f) return -1;
  fscanf(f, "%d", &pid);
  fclose(f);
  return pid;
}

void write_pid(char *path, int pid) {
  int old_pid = read_pid(path);
  if(old_pid && old_pid > 0) kill_process(old_pid);
  FILE *f = fopen(path, "w");
  fprintf(f, "%d", pid);
  fclose(f);
}

int process_running(int pid) {
  return (kill(pid, 0) == 0);
}

void restart_process(int pid) {
  kill(pid, SIGHUP);
}
void gracefully_kill_process(int pid) {
  kill(pid, SIGQUIT);
}
void kill_process(int pid) {
  kill(pid, SIGKILL);
}
