#include "unicorn_hunter.h"

#define CONFIG_PATH_PREFIX "/etc/unicorn"

unicorn_config* read_config_file(char *name) {
  char path[2048];
  char buf[1024];
  unicorn_config *conf = malloc(sizeof(unicorn_config));
  conf->keys = malloc(sizeof(char*) * 128);
  conf->values = malloc(sizeof(char*) * 128);
  conf->num_keys = 0;

  sprintf(path, "%s/unicorn_%s.conf", CONFIG_PATH_PREFIX, name);
  FILE *f = fopen(path, "r");
  if(f) {
    while(fgets(buf, 1024, f)) {
      char var[1024], val[1024];
      int i = 0, read_var = 1;
      char *p = var;

      memset(var, 0, 1024); memset(val, 0, 1024);
      if(buf[0] == '#') continue;

      while(i < strlen(buf)) {
        if(buf[i] == '\0' || buf[i] == '\n') {
          *p = '\0'; break;
        } else if(read_var && buf[i] == '=') {
          read_var = 0;
          *p = '\0';
          p = val;
          i++;
          continue;
        }
        *p = buf[i]; p++; i++;
      }

      if(read_var == 0 && strlen(var) > 0 && strlen(val) > 0) {
        int key = conf->num_keys;
        COPY_STRING(conf->keys[key], var);
        COPY_STRING(conf->values[key], val);
        conf->num_keys++;
      }
    }
    fclose(f);
    return conf;
  } else {
    printf("Can't find config file at %s.\n", path);
    exit(1);
  }
  return NULL;
}

char *config_value(unicorn_config* conf, char *key, char *default_value) {
  int i;
  for(i = 0; i < conf->num_keys; i++) {
    if(STREQ(conf->keys[i], key)) return conf->values[i];
  }
  return default_value;
}

char **unicorn_env(char *name, unicorn_config* conf) {
  char **env = malloc(sizeof(char*) * conf->num_keys+2);
  char buf[1024];
  int i;
  sprintf(buf, "APP_NAME=%s", name);
  COPY_STRING(env[0], buf);
  for(i = 0; i < conf->num_keys; i++) {
    sprintf(buf, "%s=%s", conf->keys[i], conf->values[i]);
    COPY_STRING(env[i+1], buf);
  }
  env[conf->num_keys+1] = NULL;
  return env;
}
