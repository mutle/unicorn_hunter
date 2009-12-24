#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

typedef struct unicorn_config {
  char **keys;
  char **values;
  int num_keys;
} unicorn_config;

unicorn_config* read_config_file(char *name);
char *config_value(unicorn_config* conf, char *key, char *default_value);
char **unicorn_env(char *name, unicorn_config* conf);

#endif
