#ifndef SHELL_H_
#define SHELL_H_

#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

/* Make sure the shell is running interactively as the foreground job
   before proceeding. */
void init_shell();

/* getters for shell variables */
pid_t get_shell_pgid();
struct termios* get_shell_tmodes();
int get_shell_terminal();
int get_shell_is_interactive();

#endif // SHELL_H_
