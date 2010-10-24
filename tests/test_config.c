#include <stdio.h>

#include "../rerun_config.h"

void print_exclude_files(struct rerun_config *config)
{
    int i;
    printf("printing exclude_files:");
    for (i = 0; i < config->exclude_i; i++) {
        printf(" %s", config->exclude_files[i]);
    }
    printf("\n");
}

/**
 * command for exposing rerun_config functions to test code
 *
 * valgrind ./test_config foo bar baz
 * should print
 * print_exclude_files foo bar baz\n
 * No memory leaks should be detectable.
 */
int main(int argc, char *argv[])
{
    int i;
    struct rerun_config *config = (struct rerun_config *) init_config();
    for (i = 0; i < argc; i++) {
        add_exclude_file(config, argv[i]);
        print_exclude_files(config);
    }
    printf("%d\n", config->exclude_i);
    free_config(config);
    return 0;
}
