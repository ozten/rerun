/**
 * watched_len is the index of the most recent 
 * watched directory (watched_dirs and watched_fpaths)
*/
struct inotify_state {
  int watched_len;
  int max_watched_len;
  int fd;
  int *watched_dirs;
  char **watched_fpaths;
};

void * init_inotify(char *directory); 
void rerun(struct inotify_state *state, char *fileglob, char *command);
void cleanup(void);
