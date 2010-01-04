#include "unicorn_hunter.h"

char* unicorn_hunter_pid_path(unicorn *u) {
  return u->hunter_pid_file;
}
char* unicorn_pid_path(unicorn *u) {
  return u->pid_file;
}

unicorn *create_unicorn(char *name) {
  unicorn_config *conf = read_config_file(name);
  unicorn *u = malloc(sizeof(unicorn));
  char buf[1024];
  u->pid = 0; u->num_workers = 3; u->using_bundler = 0;
  COPY_STRING(u->name, name);
  COPY_STRING(u->root, config_value(conf, "APP_ROOT", NULL));
  COPY_STRING(u->listen, config_value(conf, "UNICORN_LISTEN", "3000"));
  COPY_STRING(u->environment, config_value(conf, "APP_ENV", "production"));
  COPY_STRING(u->ruby, config_value(conf, "APP_RUBY", "ruby"));
  COPY_STRING(u->pid_path, config_value(conf, "UNICORN_PID_DIR", "/var/run/unicorn"));
  COPY_STRING(u->app_type, config_value(conf, "APP_TYPE", "rails"));
  if(STREQ(config_value(conf, "BUNDLER", "false"), "true")) u->using_bundler = 1;
  sprintf(buf, "%s/unicorn.%s.hunter.pid", u->pid_path, u->name);
  COPY_STRING(u->hunter_pid_file, buf);
  sprintf(buf, "%s/unicorn.%s.pid", u->pid_path, u->name);
  COPY_STRING(u->pid_file, buf);
  u->conf = conf;
  return u;
}

int unicorn_running(unicorn *u) {
  int pid = read_pid(unicorn_pid_path(u));
  if(pid < 1) {
    u->pid = 0;
    return 0;
  }
  if(u->pid == 0) u->pid = pid;
  if(process_running(u->pid)) return 1;
  cleanup_unicorn(u);
  return 0;
}

void monitor_unicorn(unicorn *u) {
  int pid = 0;
  if(!unicorn_running(u)) {
    start_unicorn(u);
  }
}

void start_monitoring_unicorn(unicorn *u) {
  if(!fork()) {
    write_pid(unicorn_hunter_pid_path(u), getpid());
    while(1) {
      monitor_unicorn(u);
      sleep(MONITOR_CYCLE);
    }
  }
}

void start_unicorn(unicorn *u) {
  pid_t pid = 0;
  int child_pid = 0;
  int timeout = 0;
  char unicorn_command[1024];
  pid = vfork();
  if(pid == 0) {
    // Exec unicorn in child process
    chdir(u->root);
    char **env = unicorn_env(u->name, u->conf);
    char *unicorn_binary = (STREQ(u->app_type, "rails") ? "unicorn_rails" : "unicorn");
    char *unicorn_binary_prefix = (u->using_bundler ? "bin/" : "");
    char *ruby_prefix = (!STREQ(u->ruby, "ruby") ? u->ruby : "");
    sprintf(unicorn_command, "%s %s%s -D -E %s -c config/unicorn.rb", ruby_prefix, unicorn_binary_prefix, unicorn_binary, u->environment);
    if(execle("/bin/sh", "/bin/sh", "-c", unicorn_command, NULL, env) == -1) {
      log_message("Failed to start unicorn");
    }
    exit(0);
  } else {
    // Wait till child process is started
    while(1) {
      sleep(MONITOR_CYCLE);
      timeout++;
      if(unicorn_running(u)) {
        break;
      }
      if(timeout > MONITOR_START_CYCLES) {
        log_message("Start timed out, trying again...");
        kill_process(pid);
        break;
      }
    }
  }
}

void unicorn_not_running(unicorn *u) {
  printf("Unicorn %s is not running.\n", u->name);
}

void restart_unicorn(unicorn *u, int restart_master) {
  if(!unicorn_running(u)) return unicorn_not_running(u);
  if(!restart_master)
    restart_process(u->pid);
  else {
    int old_pid = u->pid, new_pid = 0;
    stop_unicorn_hunter_monitor(u);
    log_message("Beginning restart cycle");
    kill(old_pid, SIGUSR2);
    while(1) {
      sleep(MONITOR_CYCLE);
      new_pid = read_pid(unicorn_pid_path(u));
      if(new_pid > 0 && new_pid != old_pid) {
        printf("New master running with pid %d, winching down %d\n", new_pid, old_pid);
        kill(old_pid, SIGWINCH);
        u->pid = new_pid;
        sleep(1);
        gracefully_kill_process(old_pid);
        log_message("Master restarted, monitoring again");
        start_monitoring_unicorn(u);
        return;
      } else {
        printf("Old pid found %d (old %d)\n", new_pid, old_pid);
      }
    }
  }
}

void stop_unicorn(unicorn *u) {
  if(!unicorn_running(u)) return unicorn_not_running(u);
  cleanup_unicorn(u);
}

void stop_unicorn_hunter_monitor(unicorn *u) {
  int hunter_pid = read_pid(unicorn_hunter_pid_path(u));
  if(hunter_pid > 0) {
    kill_process(hunter_pid);
    unlink(unicorn_hunter_pid_path(u));
  }
}

void cleanup_unicorn(unicorn *u) {
  int timeout = 0;
  if(u->pid > 0) {
    stop_unicorn_hunter_monitor(u);
    gracefully_kill_process(u->pid);
    while(1) {
      sleep(MONITOR_CYCLE);
      timeout++;
      if(!process_running(u->pid)) {
        printf("Killed unicorn %s (PID %d).\n", u->name, u->pid);
        break;
      }
      if(timeout > MONITOR_START_CYCLES) {
        log_message("Stop timed out, killing...");
        kill_process(u->pid);
        break;
      }
    }
  }
  unlink(unicorn_pid_path(u));
}

void set_unicorn_workers(unicorn *u, int change) {
  int i;
  int old_workers = u->num_workers;
  u->num_workers += change;
  if(!unicorn_running(u)) return unicorn_not_running(u);
  printf("Adjusting %d number of workers by %d\n", u->pid, change);
  if(change > 0) {
    kill(u->pid, SIGTTIN);
  } else if(change < 0) {
    kill(u->pid, SIGTTOU);
  }
  printf("Adjusted number of workers by %d\n", change);
}

void set_process_title(char *argv0, unicorn *u) {
  char title[255];
  int argv0size = strlen(argv0);
  sprintf(title, "unicorn %s", u->name);
  log_message(title);
  strncpy(argv0,title,argv0size);
}
