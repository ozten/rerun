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

#include <stdio.h>
#include <stdlib.h> /* exit */
#include <errno.h>


#include <string.h> /* strcmp */
#include <sysexits.h> /* EX_USAGE, EX_OK */

#include <signal.h> /* sigaction */
#include <bits/sigaction.h> /* WTF? sigaction TODO gcc -std=gnu99 instead of ansi? */

#include "rerun.h"

static void usage(char[]);
static void version(char *program);

void handler(int sig)
{
  if (sig == SIGINT) {
    printf("Got %d SIGINT quiting\n", sig);
  } else {
    printf("Got %d ? quiting\n", sig);
  }
  cleanup();
  exit(EX_OK);
}

int main(int argc, char *argv[])
{
  int i;
  char *program = argv[0];
  char *directory, *fileglob, *command;
  struct inotify_state *state;
  struct sigaction sa;

  directory = "unknown";
  command = "";
  fileglob = "*";

  if (2 == argc && 
      0 == (strcmp("--help", argv[1]))) {
    usage(program);
    return EX_USAGE;
  } else if (2 == argc && 
             (0 == (strcmp("--version", argv[1])) ||
              0 == (strcmp("-v", argv[1])))) {
    version(program);
    return EX_OK; 
  } else if (argc <= 2) {
    usage(program);
    return EX_USAGE;
  }
  for (i = 1; i < argc; i++) {
    printf("%s\n", argv[i]);
    if (i == 1) {    
      directory = argv[i];
      printf("Watching %s\n", directory);
    } else if (i == 2) {
      fileglob = argv[i];
    } else if (i == 3) {
      command = argv[i];
    }
  }
  state = init_inotify(directory);

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
  printf(
"Usage %s DIRECTORY FILE_PATTERN COMMAND...\n"
"\n"
"Watches DIRECTORY for changes in any file that matches FILE_PATTERN\n"
"and then runs COMMAND. FILE_PATTERN should be in the glob format.\n"
"\n"
"You probably want to wrap both the FILE_PATTERN and COMMAND in quotes,\n"
"so the shell doesn't pre-expand patterns and the command is seen as a \n"
"single argument.\n"
"\n"
"Examples:\n"
"\t%s web/js \"*.js\" \"juicer merge -min '' web/js/behavior.js\n"
"\n"
"\t%s web \"*.css\" \"bin/package_site.sh\n"
"\n"
"Bugs:\n"
"rerun doesn't watch directories recursively\n"
"\n", program, program, program); 
}

void version(char *program)
{
  printf(
"%s 0.1\n"
"\n"
"Mozilla Tri-License: MPL 1.1/GPL 2.0/LGPL 2.1\n"
"\n"
"Written by Austin King <shout@ozten.com>.\n", program);
}

