#include <errno.h>
#include <sysexits.h>
#include <sys/inotify.h>
#include <stdio.h>
#include <string.h> /* strerror */
#include <unistd.h> /* close */
#include <stdlib.h> /* exit */
#include <fnmatch.h>
#include <sys/wait.h> /* WTERMSIG, etc */

#include "rerun.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

static int fd, wd;

void init_inotify(char *directory)
{
  fd = inotify_init();
  if (-1 == fd) {
    perror("Unable to setup the system."); 
  }
  wd = inotify_add_watch(fd, directory, IN_MODIFY | IN_CREATE | IN_DELETE);
  /* TODO recursively walk tree adding
     any directories we see 
     TODO When a directory is deleted, rm it's watches
     TODO When a directory is added, add it's watches
   */
  if (-1 == wd) {  
    char msg[140] = "";
    sprintf(msg, "Unable to watch directory %s the system.", directory);
    perror(msg);
  }
}

void rerun(char *fileglob, char *command)
{
  char buffer[BUF_LEN];
  int length = 0;
  int inotify_index = 0;
  int ret = 0;

    printf("Sleeping\n");
    length = read( fd, buffer, BUF_LEN );
    printf("Waking up\n"); 
    printf("Read %d %d back\n", length, errno); 
    if (length < 0) {
      perror("Unknown Error while watching for changes.");
    } else {
      inotify_index = 0;
      while(inotify_index < length) {
        struct inotify_event *event = ( struct inotify_event * ) &buffer[ inotify_index ];
        if (event->len) {
          if (0 == fnmatch(fileglob, event->name, 0)) {
            printf("File was modified %s executing command %s\n", 
                   event->name, command);

            ret = system(command);

            if (WIFSIGNALED(ret) &&
                (WTERMSIG(ret) == SIGINT || WTERMSIG(ret) == SIGQUIT)) {
              cleanup_inotify();
            } 
          } else {
            /* printf("Skipping %s didn't match %s\n", event->name, fileglob); */
          }          
          inotify_index += EVENT_SIZE + event->len;
        }
      }
    } 
}

void cleanup_inotify(void)
{
  if (inotify_rm_watch(fd, wd) < 0) {
    printf("Error trying to stop watching Error:%s\n", strerror(errno));
  }
  if (close(fd) < 0) {
    printf("Error closing inotify Error:%s\n", strerror(errno));
  }
  exit(EX_OK);
}

