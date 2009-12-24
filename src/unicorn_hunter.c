#include "unicorn_hunter.h"

void log_unicorn(unicorn *u) {
  printf("Unicorn %s:\n", u->name);
  printf("  Root: %s\n", u->root);
  printf("  Environment: %s\n", u->environment);
  printf("  Pid: %d%s\n", u->pid, (u->pid == 0 ? " (not running)" : ""));
  printf("  Listen: %s\n", u->listen);
  printf("  Workers: %d\n", u->num_workers);
  printf("  Ruby executable: %s\n", u->ruby);
  printf("  Pid file: %s\n", unicorn_pid_path(u));
  printf("  Hunter Pid file: %s\n", unicorn_hunter_pid_path(u));
}

void usage(char **argv) {
  printf("usage: %s <application> {start,restart,stop,deploy,worker,status}\n", argv[0]);
  exit(1);
}
void worker_usage(char **argv) {
  printf("usage: %s <application> worker {add,remove}\n", argv[0]);
  exit(1);
}

int main(int argc, char **argv) {
  if(argc < 3) {
    usage(argv);
  }
  char *application = argv[1];
  char *command = argv[2];
  unicorn *u = create_unicorn(application);

  if(STREQ(command, "stop")) {
    stop_unicorn(u);
  } else if (STREQ(command, "worker")) {
      char *worker_command = argv[3];
      if(argc < 4) {
        worker_usage(argv);
      }
      if(STREQ(worker_command, "add")) {
        set_unicorn_workers(u, 1);
      } else if (STREQ(worker_command, "remove")) {
        set_unicorn_workers(u, -1);
      } else {
        worker_usage(argv);
      }
  } else if(STREQ(command, "status")) {
    unicorn_running(u);
    log_unicorn(u);
  } else if (STREQ(command, "restart")) {
    restart_unicorn(u, 0);
  } else if (STREQ(command, "deploy")) {
    restart_unicorn(u, 1);
  } else if(STREQ(command, "start")) {
    start_monitoring_unicorn(u);
  }
  return 0;
}
