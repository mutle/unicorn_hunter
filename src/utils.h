#ifndef __UTILS_H__
#define __UTILS_H__

void log_message(char *message);

int read_pid(char *path);
void write_pid(char *path, int pid);

void restart_process(int pid);
void gracefully_kill_process(int pid);
void kill_process(int pid);
void kill_process_signal(int pid, int sig);
int process_running(int pid);

#endif
