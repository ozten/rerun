#include "rerun_config.h"
#include <stdlib.h>
#include <string.h>

void * init_config()
{
  struct rerun_config *config = (struct rerun_config*) malloc(sizeof(struct rerun_config));
  config->once = 0;
  /*exclude_files unallocated, will grow in add_exclude_file*/
  config->exclude_i = 0;
  return config;
}

void add_exclude_file(struct rerun_config *config, char *exclude)
{
  char **t_files = (char **) malloc(sizeof(char**) * (config->exclude_i + 1));
  int i;

  for (i = 0; i < config->exclude_i; i++) {
    t_files[i] = malloc(strlen(config->exclude_files[i]) + 1);
    strcpy(t_files[i], config->exclude_files[i]);
    free(config->exclude_files[i]);
  }  
  t_files[i] = malloc(strlen(exclude) + 1);
  strcpy(t_files[i], exclude);

  if (config->exclude_i > 0) {
    free(config->exclude_files);
  }
  ++config->exclude_i;
  config->exclude_files = (char **) malloc(sizeof(char**) * config->exclude_i);

  for (i = 0; i < config->exclude_i; i++) {
    config->exclude_files[i] = (char *) malloc(sizeof(char) * (strlen(t_files[i]) + 1));
    strcpy(config->exclude_files[i], t_files[i]);
    free(t_files[i]);
  }
  free(t_files);  
}

void free_config(struct rerun_config* config)
{
  int i;
  for (i = 0; i < config->exclude_i; i++) {
    free(config->exclude_files[i]);
  }
  free(config->exclude_files);
  free(config);
}
