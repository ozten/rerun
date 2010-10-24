#ifndef FILE_RERUN_CONFIG_SEEN
#define FILE_RERUN_CONFIG_SEEN

struct rerun_config {
  int once;
  char **exclude_files;
  int exclude_i;
};

/**
 * Allocates memory for config and sets default values
 * struct rerun_config*
 */
void * init_config(void);

/**
 * Resizes config and adds excluded filepattern
 */
void add_exclude_file(struct rerun_config *config, char *exclude);

/**
 * Frees up memory used by the config
 */
void free_config(struct rerun_config* config);

#endif /* !FILE_RERUN_CONFIG_SEEN */
