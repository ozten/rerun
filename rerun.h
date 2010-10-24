#ifndef FILE_RERUN_SEEN
#define FILE_RERUN_SEEN

#include "rerun_config.h"

/**
 * watched_len is the index of the most recent 
 * watched directory (watched_dirs and watched_fpaths)
 * in_use list of flags if this dirs is being watched, or was removed previously
*/
struct inotify_state {
  int watched_len;
  int max_watched_len;
  int fd;
  int *watched_dirs;
  char **watched_fpaths;
  int *in_use;
  struct rerun_config *config;
};

void * init_inotify(char *directory, struct rerun_config *config); 
void rerun(struct inotify_state *state, char *fileglob, char *command);
void cleanup(void);

#endif /* !FILE_RERUN_SEEN */
