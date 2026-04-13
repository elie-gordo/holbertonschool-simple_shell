#ifndef SHELL_H
#define SHELL_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int simple_shell(char *prog_name, char **envp);
int execute_word(char *word, char *prog_name, char **envp);

#endif
