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
/*
#define _POSIX_C_SOURCE 2
#define _XOPEN_SOURCE
#define _POSIX_SOURCE
*/

#include <stdio.h>
#include <stdlib.h> /* exit */
#include <errno.h>


#include <string.h> /* strcmp */
#include <sysexits.h> /* EX_USAGE, EX_OK */ 

#include <unistd.h>
#include <signal.h> /* sigaction */
#include <bits/sigaction.h> /* WTF? sigaction TODO gcc -std=gnu99 instead of ansi? */

#include "rerun.h"

static void usage(char[]); 
static void version(char *program);

void handler(int sig)
{
  if (sig == SIGINT) {
    fprintf(stdout, "Got %d SIGINT quiting\n", sig);
  } else {
    fprintf(stdout, "Got %d ? quiting\n", sig);
  }
  cleanup();
  exit(EX_OK);
}

int main(int argc, char *argv[])
{
  int i = 1, in_options = 1;
  char *program = argv[0];
  char *directory, *fileglob, *command;
  struct inotify_state *state;
  struct sigaction sa;
  struct rerun_config config = {0};
  
  directory = "unknown";
  command = "";
  fileglob = "*";

  while(1) {
    if ('-' == argv[i][0]) {
      if (0 == (strcmp("--once", argv[i]))) {
        config.once = 1;
      } else if (0 == (strcmp("--help", argv[i]))) {
        usage(program);
        return EX_USAGE;
      } else if (0 == (strcmp("--version", argv[i])) ||
                 0 == (strcmp("-v", argv[i]))) {
        version(program);
        return EX_USAGE;
      }
      i++;
    } else {
      break;
    }
  }
  fprintf(stdout, "%d arguments left\n", (argc - i));
  if (2 >= (argc - i)) { 
    usage(program);
    return EX_USAGE;
  }
  directory = argv[i++];
  fprintf(stdout, "Watching %s\n", directory);
  fileglob = argv[i++];
  fprintf(stdout, "for %s\n", fileglob);
  command = argv[i++];
  fprintf(stdout, "to eventually run %s\n", command);
  state = init_inotify(directory, config);

  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  if (sigaction(SIGHUP, &sa, NULL) == -1) {
    perror("Unable to setup sigaction");    
  }
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("Unable to setup sigaction");    
  }
  while(1) {
    rerun(state, fileglob, command);
  }
  cleanup_inotify(state);
  return EX_OK; /* Should never get here */
}

void usage(char program[])
{
/*2345678901234567890123456789012345678901234567890123456789012345678901234567*/
  fprintf(stdout, 
"Usage %s [OPTIONS] DIRECTORY FILE_PATTERN COMMAND...\n"
"\n"
"Watches DIRECTORY recursively for changes in any file that matches FILE_PATTERN\n"
"and then runs COMMAND. FILE_PATTERN should be in the glob format.\n"
"\n"
"You probably want to wrap both the FILE_PATTERN and COMMAND in quotes,\n"
"so the shell doesn't pre-expand patterns and the command is seen as a \n"
"single argument.\n"
"\n"
"Options:\n"
"\t--once\tRun until a change is detected, then exit\n"
"\n"
"Examples:\n"
"\t%s web/js \"*.js\" \"juicer merge -min '' web/js/behavior.js\n"
"\n"
"\t%s web \"*.css\" \"bin/package_site.sh\n"
"\n"
"rerun will ignore dotfiles such as .#foo.js. It will watch new sub-\n"
"directories which are added later.\n"
"\n"
"Bugs:\n"
"When a directory is deleted, it's watches are not removed.\n"
"Adding and deleting a lot of sub-directories rapidly may confuse rerun.\n"
"\n", program, program, program); 
}

void version(char *program)
{
  fprintf(stdout, 
"%s 0.2\n"
"\n"
"Mozilla Tri-License: MPL 1.1/GPL 2.0/LGPL 2.1\n"
"\n"
"Written by Austin King <shout@ozten.com>.\n", program);
}

