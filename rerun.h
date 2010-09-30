struct inotify_state {
  int fd;
  int wd;
};

void * init_inotify(char *directory); 
void rerun(struct inotify_state *state, char *fileglob, char *command);
void cleanup_inotify(struct inotify_state *state);

