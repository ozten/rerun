/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is a Mozilla Webdev CLI tool
 *
 * The Initial Developer of the Original Code is
 * Mozilla Corporation.
 * Portions created by the Initial Developer are Copyright (C) 2010
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *    Austin King <shout@ozten.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#define _XOPEN_SOURCE 500 /* ftw */
#include <limits.h> /* realpath */
#include <stdlib.h>
#include <errno.h>
#include <sysexits.h>
#include <sys/inotify.h>
#include <stdio.h>
#include <string.h> /* strerror, strcpy */
#include <unistd.h> /* close */
#include <stdlib.h> /* exit, realpath */
#include <fnmatch.h>
#include <sys/wait.h> /* WTERMSIG, etc */
#include <ftw.h>

#include "rerun.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

struct inotify_state *rerun_state; /* only used by ftw, cleanup */
void grow_watched_dirs(struct inotify_state *state);
static void cleanup_inotify(struct inotify_state *state);
static void watch_directory(struct inotify_state *state, char *directory);

static int visit_directory(const char *fpath, const struct stat *sb,
             int tflag, struct FTW *ftwbuf)
{
  char *path;
  printf("enter visit_directory\n");
  if (tflag == FTW_D) {
    path = realpath(fpath, NULL); 
    if (path != NULL) {
      printf("Watching %s\n", path);
      watch_directory(rerun_state, path);
      free(path);
      path = NULL;
    }
  }
  printf("exit visit_directory\n");

  return 0;
}

void watch_directory(struct inotify_state *state, char *directory)
{
  printf("enter watch_directory\n");
  if ((state->watched_len + 1) == state->max_watched_len) {
    grow_watched_dirs(state);
  }
  printf("watch_dir %d\n", state->watched_len);
  state->watched_len++;
  printf("watch_dir %d\n", state->watched_len);

  state->watched_dirs[state->watched_len] = 
    inotify_add_watch(state->fd, directory, IN_MODIFY | IN_CREATE | IN_DELETE);

  if (-1 == state->watched_dirs[state->watched_len]) {
    char *msg = (char *) malloc(sizeof(char *) * \
                                (strlen("Unable to watch directory %s the system.") +
                                 strlen(directory)));
                                                 ; /* TODO use strlen here */
    sprintf(msg, "Unable to watch directory %s the system.", directory);
    perror(msg); 
    free(msg);
    msg = NULL;
  } else {

    state->watched_fpaths[state->watched_len] = 
      (char *) malloc(sizeof(char) * (strlen(directory) + 1));
    strcpy(state->watched_fpaths[state->watched_len], directory);
  } 
  printf("exit watch_directory\n"); 
}

void grow_watched_dirs(struct inotify_state *state)
{
  int  *t_wds;
  char **t_fpaths;
  int i;
  int grow_size = 5;

  /* 0 through watched_len (inclusive) are have valid data, the rest 
     are unallocated memory */

  printf("DEBUG: ENTER grow_watched_dirs watched_len=[%d]\n", state->watched_len);
  t_wds    = (int *)  malloc(sizeof(int) *  (state->max_watched_len + grow_size));
  /* TODO: aok t_fpaths could be the same size as old state */
  t_fpaths = (char **) malloc(sizeof(char *) * (state->max_watched_len + grow_size));
  if (t_wds != NULL && t_fpaths != NULL) {
    for (i=0; i <= state->watched_len; i++) {
        /* TODO try using memcpy(t_wds, state->watched_dirs); instead */
        t_wds[i]    = state->watched_dirs[i];

        t_fpaths[i] =
          (char *) malloc(sizeof(char) * (strlen(state->watched_fpaths[i]) + 1));
          strcpy(t_fpaths[i], state->watched_fpaths[i]);      

          free(state->watched_fpaths[i]);
          state->watched_fpaths[i] = NULL;
    }

    free(state->watched_dirs);
    state->watched_dirs = NULL;
    free(state->watched_fpaths);
    state->watched_fpaths = NULL;

    state->watched_dirs = (int *) malloc(sizeof(int) * 
                                         (state->max_watched_len + grow_size));
    state->watched_fpaths = (char **) malloc(sizeof(char *) * (state->max_watched_len + grow_size));

    for (i=0; i <= state->watched_len; i++) {
        state->watched_dirs[i] = t_wds[i]; 
        state->watched_fpaths[i] =
            (char *) malloc(sizeof(char) * (strlen(t_fpaths[i]) + 1));
          strcpy(state->watched_fpaths[i], t_fpaths[i]);
          free(t_fpaths[i]);
          t_fpaths[i] = NULL;
    }
    free(t_wds);
    t_wds = NULL;
    free(t_fpaths);
    t_fpaths = NULL;

    state->max_watched_len = state->max_watched_len + grow_size;
  } else {
    perror("Unable to allocate memory to grow watched directories\n");
  }
  printf("DEBUG: EXIT grow_watched_dirs\n");
}

void * init_inotify(char *directory)
{

  struct inotify_state *state;

  printf("enter init_inotify\n");
  rerun_state = state = (struct inotify_state *) malloc(sizeof(struct inotify_state));
  state->watched_len = -1;/* when used, increment then use */
  state->max_watched_len = 5;
  state->watched_dirs   = (int *) malloc(sizeof(int) * state->max_watched_len);
  state->watched_fpaths = (char **) malloc(sizeof(char *) * state->max_watched_len);

  state->fd = inotify_init();
  if (-1 == state->fd) {
    perror("Unable to setup the system."); 
  }
  
  printf("after watch_dir %d\n", state->watched_len);
  
  if (nftw(directory, visit_directory, 20, 0) == -1) {
    perror("nftw");
    exit(EXIT_FAILURE);
    }
  printf("finished ftw\n");
  /* TODO recursively walk tree adding
     any directories we see 
     TODO When a directory is deleted, rm it's watches
     TODO When a directory is added, add it's watches
   */
  printf("exit init_inotify\n");
  return state;
}

void rerun(struct inotify_state *state, char *fileglob, char *command)
{

  char buffer[BUF_LEN];
  int length = 0;
  int inotify_index = 0;
  int ret = 0;
  printf("enter rerun\n");
    printf("Sleeping\n");
    length = read( state->fd, buffer, BUF_LEN );
    printf("Waking up\n"); 
    printf("Read %d %d back\n", length, errno); 
    if (length < 0) {
      perror("Unknown Error while watching for changes.");
      exit(EX_UNAVAILABLE);
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
              cleanup_inotify(state);
            } 
          } else {
            printf("Skipping %s didn't match %s\n", event->name, fileglob);
          }          
          inotify_index += EVENT_SIZE + event->len;
        }
      }
    } 
}

void cleanup(void)
{
  cleanup_inotify(rerun_state);
}

void cleanup_inotify(struct inotify_state *state)
{

  int i;
  printf("enter cleanup_inotify [%d]\n", state->watched_len);
  for (i=0; i <= state->watched_len; i++) {
    printf("DEBUG: Removing watch [%s]\n", state->watched_fpaths[i]);
    if (inotify_rm_watch(state->fd, state->watched_dirs[i]) < 0) {
      printf("ERROR: trying to stop watching Error:%s\n", strerror(errno));
    } else {
      printf("DEBUG: Removed watch\n");
    }
    free(state->watched_fpaths[i]);
    state->watched_fpaths[i] = NULL;
  }
  free(state->watched_dirs);
  free(state->watched_fpaths);
  if (close(state->fd) < 0) {
    printf("Error closing inotify Error:%s\n", strerror(errno));
  }
  free(state);
  state = NULL;
  printf("exit cleanup_inotify\n");
  exit(EX_OK);
}

