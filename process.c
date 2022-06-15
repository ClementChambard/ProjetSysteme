#include "process.h"
#include "shell.h"
#include "util.h"
#include "builtin.h"
#include "job.h"
#include "interface.h"
#include "cp/cp.h"
#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

void free_process(process* p)
{
    if (p->next) free_process(p->next);
    char** a = p->argv;
    while (*a) free(*(a++));
    free(p->argv);
    free(p);
}

int check_builtin_process(char** argv)
{   
    int argc = 0;
    char** av = argv;
    while (*(av++)) argc++;
    if (argv == NULL) return 1;
    if (!strcmp(argv[0],"cd")) { cd(++argv); return 1; }
    if (!strcmp(argv[0],"reload")) { reload(++argv); return 1; }
    if (!strcmp(argv[0],"recompile")) { recompile(++argv); return 1; }
    if (!strcmp(argv[0],"cp")) { cp(argc, argv); return 1; }
    if (!strcmp(argv[0],"alias")) { alias(++argv); return 1; }
    //if (!strcmp(argv[0],"jobs")) { jobs(argc, argv); return 1; }
    //if (!strcmp(argv[0],"kill")) { kill_job(argc, argv); return 1; }
    
    return 0;
}

void launch_process(process *p, pid_t pgid, int infile, int outfile, int errfile, int foreground)
{

    pid_t pid;

    if (get_shell_is_interactive())
    {
        /* Put the process into the process group and give the process group
           the terminal, if appropriate.
           This has to be done both by the shell and in the individual
           child processes because of potential race conditions.  */
        pid = getpid();
        if (pgid == 0) pgid = pid;
        setpgid(pid, pgid);
        if (foreground) tcsetpgrp(get_shell_terminal(), pgid);

        /* Set the handling for job control signals back to the default.  */
        signal(SIGINT , SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);
    }

    /* Set the standard input/output channels of the new process.  */
    if (infile != STDIN_FILENO)
    {
        dup2(infile, STDIN_FILENO);
        close(infile);
    }
    if (outfile != STDOUT_FILENO)
    {
        dup2(outfile, STDOUT_FILENO);
        close(outfile);
    }
    if (errfile != STDERR_FILENO)
    {
        dup2(errfile, STDERR_FILENO);
        close(errfile);
    }

    /* Exec the new process.  Make sure we exit.  */
    execvp(p->argv[0], p->argv);
    perror("execvp");
    job* j = get_first_job();
    job* j1;
    while(j) {j1 = j; j = j->next; free_job(j1);}
    free(getAnwser());
    free_config();
    exit(1);
}
