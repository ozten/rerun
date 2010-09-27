#include <stdio.h>

#include <errno.h>


#include <string.h> /* strcmp */
#include <sysexits.h> /* EX_USAGE, EX_OK */

#include "rerun.h"

static void usage(char[]);
static void version(char *program);

int main(int argc, char *argv[])
{
  int i;
  char *program = argv[0];
  char *directory, *fileglob, *command;

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
  init_inotify(directory);
  while(1) {
    rerun(fileglob, command);
  }/*while(1)*/
  cleanup_inotify();
  return EX_OK; /* Should never get here */
}

void usage(char program[])
{
/*2345678901234567890123456789012345678901234567890123456789012345678901234567*/
  printf(
"Usage %s DIRECTORY -f FILE_PATTERN  do COMMAND...\n"
"     %s FILES ... do COMMAND\n"
"\n"
"Watches DIRECTORY for changes in any file that matches FILE_PATTERN\n"
"and then runs COMMAND.\n"
"\n"
"Default directory is the current directory.\n"
"\n", program, program);
}

void version(char *program)
{
  printf(
"%s 0.1\n"
"\n"
"Copyright (c) 2010 Austin King\n"
"License\n"
"\n"
"Written by ozten <shout@ozten.com>.\n", program);
}

