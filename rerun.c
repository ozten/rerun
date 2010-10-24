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
#include "rerun_config.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

struct inotify_state *rerun_state; /* only used by ftw, cleanup */
void grow_watched_dirs(struct inotify_state *state);
static void cleanup_inotify(struct inotify_state *state);
static void watch_directory(struct inotify_state *state, char *directory);

int ignore_file(struct inotify_state *state, char *filename);

void handle_modified_file(struct inotify_state *state, struct inotify_event *event, char *command);
void handle_create_directory(struct inotify_state *state, struct inotify_event *event);
void handle_delete_directory(struct inotify_state *state, struct inotify_event *event);



static int visit_directory(const char *fpath, const struct stat *sb,
                           int tflag, struct FTW *ftwbuf)
{
  char *path;
  if (tflag == FTW_D) {
    path = realpath(fpath, NULL); 
    if (path != NULL) {
      watch_directory(rerun_state, path);
      free(path);
      path = NULL;
    }
  }
  return 0;
}

void watch_directory(struct inotify_state *state, char *directory)
{
  if ((state->watched_len + 1) == state->max_watched_len) {
    grow_watched_dirs(state);
  }
  state->watched_len++;

  state->watched_dirs[state->watched_len] = 
    inotify_add_watch(state->fd, directory, IN_MODIFY | IN_CREATE | IN_DELETE);
  if (-1 == state->watched_dirs[state->watched_len]) {
    char *msg = (char *) malloc(sizeof(char *) * \
                                (strlen("Unable to watch directory %s the system.") +
                                 strlen(directory)));
    sprintf(msg, "Unable to watch directory %s the system.", directory);
    perror(msg); 
    free(msg);
    msg = NULL;
  } else {
    state->in_use[state->watched_len] = 1;
    state->watched_fpaths[state->watched_len] = 
      (char *) malloc(sizeof(char) * (strlen(directory) + 1));
    strcpy(state->watched_fpaths[state->watched_len], directory);
  } 
}

void grow_watched_dirs(struct inotify_state *state)
{
  int  *t_wds;
  int  *t_in_use;
  char **t_fpaths;
  int i, n;
  int grow_size = 5;

  /* 0 through watched_len (inclusive) are have valid data, the rest 
     are unallocated memory */
  n = state->max_watched_len;
  t_wds    = (int *)   malloc(sizeof(int) * n);
  t_in_use = (int *)   malloc(sizeof(int) * n);
  t_fpaths = (char **) malloc(sizeof(char *) * n);
  if (t_wds != NULL && t_in_use != NULL && t_fpaths != NULL) {
    memcpy(t_wds, state->watched_dirs, sizeof(int *) * n);
    memcpy(t_in_use, state->in_use, sizeof(int *) * n);
    for (i=0; i <= state->watched_len; i++) {
      *t_fpaths = (char *) malloc(sizeof(char) * (strlen(*state->watched_fpaths) + 1));
      strcpy(*t_fpaths++, *state->watched_fpaths);
      free(*state->watched_fpaths);
      *state->watched_fpaths++ = NULL;
    }
    t_fpaths -= i;
    state->watched_fpaths -= i;

    free(state->watched_dirs);
    free(state->in_use);
    state->watched_dirs = NULL;
    state->in_use = NULL;
    free(state->watched_fpaths);
    state->watched_fpaths = NULL;

    state->watched_dirs = (int *) malloc(sizeof(int) * (n + grow_size));
    state->in_use = (int *) malloc(sizeof(int) * (n + grow_size));
    state->watched_fpaths = (char **) malloc(sizeof(char *) * (n + grow_size));
    memcpy(state->watched_dirs, t_wds, sizeof(int *) * (n + grow_size));
    memcpy(state->in_use, t_in_use, sizeof(int *) * (n + grow_size));
    for (i=0; i <= state->watched_len; i++) {
      *state->watched_fpaths = (char *) malloc(sizeof(char) * (strlen(*t_fpaths) + 1));
      strcpy(*state->watched_fpaths++, *t_fpaths);
      free(*t_fpaths);
      *t_fpaths++ = NULL;
    }
    t_fpaths -= i;
    state->watched_fpaths -= i;

    free(t_wds);
    free(t_in_use);
    t_wds = NULL;
    t_in_use = NULL;
    free(t_fpaths);
    t_fpaths = NULL;

    state->max_watched_len = state->max_watched_len + grow_size;
  } else {
    perror("Unable to allocate memory to grow watched directories\n");
  }
}


void * init_inotify(char *directory, struct rerun_config *config)
{
  struct inotify_state *state;

  rerun_state = state = (struct inotify_state *) malloc(sizeof(struct inotify_state));
  state->config = config;
  state->watched_len = -1;/* when used, increment then use */
  state->max_watched_len = 5;
  state->watched_dirs   = (int *) malloc(sizeof(int) * state->max_watched_len);
  state->in_use   = (int *) malloc(sizeof(int) * state->max_watched_len);
  state->watched_fpaths = (char **) malloc(sizeof(char *) * state->max_watched_len);

  state->fd = inotify_init();
  if (-1 == state->fd) {
    perror("Unable to setup the system."); 
  }

  if (nftw(directory, visit_directory, 20, 0) == -1) {
    perror("nftw");
    exit(EXIT_FAILURE);
  }
  return state;
}

void rerun(struct inotify_state *state, char *fileglob, char *command)
{
  char buffer[BUF_LEN];
  int length = 0;
  int inotify_index = 0;
  
  length = read( state->fd, buffer, BUF_LEN );
  if (length < 0) {
    perror("Unknown Error while watching for changes.");
    exit(EX_UNAVAILABLE);
  } else {
    inotify_index = 0;
    while(inotify_index < length) {
      struct inotify_event *event = ( struct inotify_event * ) &buffer[ inotify_index ];
      if (event->len) {

        if (IN_ACCESS & event->mask) {
          fprintf(stdout, "event %d, %s is IN_ACCES\n", event->wd, event->name);
        } 
        if (IN_ATTRIB & event->mask) {
          fprintf(stdout, "event %d, %s is IN_ATTRIB\n", event->wd, event->name);
        }  
        if ( IN_CLOSE_WRITE & event->mask) {
          fprintf(stdout, "event %d, %s is IN_CLOSE_WRITE\n", event->wd, event->name);
        } 
        if ( IN_CLOSE_NOWRITE & event->mask) {
          fprintf(stdout, "event %d, %s is IN_CLOSE_NOWRITE\n", event->wd, event->name);
        } 
        if ( IN_CREATE & event->mask) {
          fprintf(stdout, "event %d, %s is IN_CREATE\n", event->wd, event->name);
        } 
        if ( IN_DELETE & event->mask) {
          fprintf(stdout, "event %d, %s is IN_DELETE\n", event->wd, event->name);
        } 
        if ( IN_DELETE_SELF & event->mask) {
          fprintf(stdout, "event %d, %s is IN_DELETE_SELF\n", event->wd, event->name);
        } 
        if ( IN_MODIFY & event->mask) {
          fprintf(stdout, "event %d, %s is IN_MODIFY\n", event->wd, event->name);
        } 
        if ( IN_MOVE_SELF & event->mask) {
          fprintf(stdout, "event %d, %s is IN_MOVE_SELF\n", event->wd, event->name);
        } 
        if ( IN_MOVED_FROM & event->mask) {
          fprintf(stdout, "event %d, %s is IN_MOVED_FROM\n", event->wd, event->name);
        } 
        if ( IN_MOVED_TO & event->mask) {
          fprintf(stdout, "event %d, %s is IN_MOVED_TO\n", event->wd, event->name);
        } 
        if ( IN_OPEN & event->mask) {
          fprintf(stdout, "event %d, %s is IN_OPEN\n", event->wd, event->name);
        }
        if ( IN_IGNORED & event->mask) {
          fprintf(stdout, "event %d, %s is IN_IGNORED\n", event->wd, event->name);
        }
        if ( IN_Q_OVERFLOW & event->mask) {
          fprintf(stdout, "event %d, %s is IN_Q_OVERFLOW\n", event->wd, event->name);
        }

        if ((IN_CREATE & event->mask) &&
            (IN_ISDIR  & event->mask)) {
          handle_create_directory(state, event);
        }
        
        if ((IN_MODIFY & event->mask) &&
            (strlen(event->name) > 0 && /* We should ignore .#foo.c */
             event->name[0] != '.') &&  /* and other hidden files  */
            0 == fnmatch(fileglob, event->name, 0)) {
          if (! ignore_file(state, event->name)) {
            handle_modified_file(state, event, command);
          }
        } 
        if ((IN_DELETE_SELF & event->mask) &&
            (IN_ISDIR  & event->mask)) {
          /* TODO: if I delete a bunch of directories, I old get notified
             of the last one. Is that lost data in the queue,
             a timing issue between ftw and disk, or ??? 
             mkdir -p a/b/c/d/e/f/g (see create b, create c)
             rm -Rf a/b (see delete c, delete b)            

             Adding sleep(1) in handle_create_directory block
             let's use ftw a/b/c/d/e/f/g
             but rm sees only delete g

             Adding sync() in same area has same effect as sleep.
          */
          handle_delete_directory(state, event);
        }
        inotify_index += EVENT_SIZE + event->len;
      }
    }
  }
}

int ignore_file(struct inotify_state *state, char *filename)
{
  int i, should_exclude = 0;
  for (i = 0; i < state->config->exclude_i; i++) {
    if (0 == fnmatch(state->config->exclude_files[0], filename, 0)) {
      should_exclude = 1;
    }
  }
  return should_exclude;
}

/**
 * Called when an *interesting* file has been modified
 */
void handle_modified_file(struct inotify_state *state, struct inotify_event *event, char *command)
{
  int ret = 0;
  fprintf(stdout, "DEBUG: File was modified %s executing command %s\n", 
         event->name, command);

  ret = system(command);

  if (WIFSIGNALED(ret) &&
      (WTERMSIG(ret) == SIGINT || WTERMSIG(ret) == SIGQUIT)) {
    cleanup_inotify(state);
  } else if (state->config->once) {
    cleanup_inotify(state); 
  }
}

void handle_create_directory(struct inotify_state *state, struct inotify_event *event)
{
  int i, found;
  char *filename;

  found = 0;

  fprintf(stdout, "DEBUG: Dir being created. wd = [%d], name = [%s]\n", event->wd, event->name);
  for (i=0; i<state->watched_len; i++) {
    if (event->wd == state->watched_dirs[i]) {
      filename = (char *) malloc(sizeof(char *) * (strlen(state->watched_fpaths[i]) +
                                                   strlen(event->name) + 2)); /* / and \0 */
      strcpy(filename, state->watched_fpaths[i]);
      strcat(filename, "/");
      strcat(filename, event->name);
      if (nftw(filename, visit_directory, 20, 0) == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
      }
      free(filename);
      found = 1;
      break;
    }
  }
  if (1 != found) {
    fprintf(stderr, "Error: New Directory %s to be under a watched dir\n", event->name);
  }
}

void handle_delete_directory(struct inotify_state *state, struct inotify_event *event)
{
  
}

void cleanup(void)
{
  cleanup_inotify(rerun_state);
}

void cleanup_inotify(struct inotify_state *state)
{
  int i;
  for (i=0; i <= state->watched_len; i++) {
    fprintf(stdout, "DEBUG: Removing watch %d, [%s]\n", state->in_use[i], state->watched_fpaths[i]);
    if (1 != state->in_use[i]) {
      fprintf(stdout, "DEBUG: Skipping unwatched directory\n");
    } else {
      if (inotify_rm_watch(state->fd, state->watched_dirs[i]) < 0) {
        fprintf(stderr, "ERROR: trying to stop watching Error:%s\n", strerror(errno));
      }
    }
    free(state->watched_fpaths[i]);
    state->watched_fpaths[i] = NULL;
  }
  free(state->watched_dirs);
  free(state->in_use);
  free(state->watched_fpaths);
  if (close(state->fd) < 0) {
    fprintf(stderr, "Error closing inotify Error:%s\n", strerror(errno));
  }
  free_config(state->config);
  free(state);
  state = NULL;
  exit(EX_OK);
}
