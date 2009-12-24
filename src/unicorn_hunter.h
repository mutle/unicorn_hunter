#ifndef __UNICORN_HUNTER_H__
#define __UNICORN_HUNTER_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

#include "utils.h"
#include "configuration.h"

#define MONITOR_CYCLE 5
#define MONITOR_START_CYCLES 6

#define COPY_STRING(target, src) target = malloc(strlen(src) + 1); strcpy(target, src);
#define STREQ(str1, str2) strcmp(str1, str2) == 0


typedef struct unicorn {
  int pid;
  int num_workers;
  char *name;
  char *listen;
  char *root;
  char *environment;
  char *ruby;
  char *pid_path;
  char *hunter_pid_file;
  char *pid_file;
  unicorn_config *conf;
} unicorn;



unicorn *create_unicorn(char *name);
void monitor_unicorn(unicorn *u);
void start_unicorn(unicorn *u);
void start_monitoring_unicorn(unicorn *u);
void restart_unicorn(unicorn *u, int restart_master);
void stop_unicorn(unicorn *u);
void stop_unicorn_hunter_monitor(unicorn *u);
void cleanup_unicorn(unicorn *u);
void set_unicorn_workers(unicorn *u, int change);
char* unicorn_hunter_pid_path(unicorn *u);
char* unicorn_pid_path(unicorn *u);

#endif
