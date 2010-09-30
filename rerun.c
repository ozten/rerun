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

void * init_inotify(char *directory)
{
  struct inotify_state *state;
  state = (struct inotify_state *) malloc(sizeof(struct inotify_state));
  state->fd = inotify_init();
  if (-1 == state->fd) {
    perror("Unable to setup the system."); 
  }
  state->wd = inotify_add_watch(state->fd, directory, IN_MODIFY | IN_CREATE | IN_DELETE);
  /* TODO recursively walk tree adding
     any directories we see 
     TODO When a directory is deleted, rm it's watches
     TODO When a directory is added, add it's watches
   */
  if (-1 == state->wd) {  
    char msg[140] = ""; 
    sprintf(msg, "Unable to watch directory %s the system.", directory);
    perror(msg); 
  }
  return state;
}

void rerun(struct inotify_state *state, char *fileglob, char *command)
{
  char buffer[BUF_LEN];
  int length = 0;
  int inotify_index = 0;
  int ret = 0;

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

void cleanup_inotify(struct inotify_state *state)
{
  if (inotify_rm_watch(state->fd, state->wd) < 0) {
    printf("Error trying to stop watching Error:%s\n", strerror(errno));
  }
  if (close(state->fd) < 0) {
    printf("Error closing inotify Error:%s\n", strerror(errno));
  }
  free(state);
  state = NULL;
  exit(EX_OK);
}

