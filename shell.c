#include "shell.h"

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
int shell_is_interactive;

pid_t get_shell_pgid() { return shell_pgid; }
struct termios* get_shell_tmodes() { return &shell_tmodes; }
int get_shell_terminal() { return shell_terminal; }
int get_shell_is_interactive() { return shell_is_interactive; }


void init_shell()
{

  /* See if we are running interactively.  */
  shell_terminal = STDIN_FILENO;
  shell_is_interactive = isatty(shell_terminal);

  if (shell_is_interactive)
  {
      /* Loop until we are in the foreground.  */
      while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
          kill(- shell_pgid, SIGTTIN);

      /* Ignore interactive and job-control signals.  */
      signal(SIGINT, SIG_IGN);
      signal(SIGQUIT, SIG_IGN);
      signal(SIGTSTP, SIG_IGN);
      signal(SIGTTIN, SIG_IGN);
      signal(SIGTTOU, SIG_IGN);
      //signal(SIGCHLD, SIG_IGN);

      /* Put ourselves in our own process group.  */
      shell_pgid = getpid();
      if (setpgid(shell_pgid, shell_pgid) < 0)
      {
          perror("Couldn't put the shell in its own process group");
          exit(1);
      }

      /* Grab control of the terminal.  */
      tcsetpgrp(shell_terminal, shell_pgid);

      /* Save default terminal attributes for shell.  */
      tcgetattr(shell_terminal, &shell_tmodes);
  }
}
