#include "job.h"
#include "process.h"
#include "shell.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

job* first_job = NULL;

job* get_first_job() { return first_job; }

job* new_job()
{
    job* j;
    if (first_job == NULL)
        j = first_job = malloc(sizeof(job));
    else
    {
        j = first_job;
        while (j->next) j = j->next;
        j = j->next = malloc(sizeof(job));
    }
    j->next = NULL;
    j->command = NULL;
    j->first_process = NULL;
    j->notified = 0;
    j->pgid = 0;
    j->stderr = STDERR_FILENO;
    j->stdin = STDIN_FILENO;
    j->stdout = STDOUT_FILENO;
    return j;
}

process* add_process(job* j)
{
    process* p;
    if (j->first_process == NULL)
        p = j->first_process = malloc(sizeof(process));
    else
    {
        p = j->first_process;
        while (p->next) p = p->next;
        p = p->next = malloc(sizeof(process));
    }
    p->argv = NULL;
    p->completed = 0;
    p->next = NULL;
    p->pid = 0;
    p->status = 0;
    p->stopped = 0;
    return p;
}

void free_job(job* j)
{
    free(j->command);
    free_process(j->first_process);
    free(j);
}

job* find_job(pid_t pgid)
{
    job* j;

    for (j = first_job; j; j = j->next)
        if (j->pgid == pgid)
            return j;
    return NULL;
}

int job_is_stopped(job* j)
{
    process* p;

    for (p = j->first_process; p; p = p->next)
        if (!p->completed && !p->stopped)
            return 0;
    return 1;
}

int job_is_completed(job* j)
{
    process* p;

    for (p = j->first_process; p; p = p->next)
        if (!p->completed)
            return 0;
    return 1;
}

void launch_job(job *j, int foreground)
{
    process *p;
    pid_t pid;
    int mypipe[2], infile, outfile;

    infile = j->stdin;
    for (p = j->first_process; p; p = p->next)
    {
        /* Set up pipes, if necessary.  */
        if (p->next)
        {
            if (pipe(mypipe) < 0)
            {
                perror("pipe");
                exit(1);
            }
            outfile = mypipe[1];
        }
        else outfile = j->stdout;

        if (!check_builtin_process(p->argv))
        {
            /* Fork the child processes.  */
            pid = fork();
            if (pid == 0)
                /* This is the child process.  */
                launch_process(p, j->pgid, infile, outfile, j->stderr, foreground);
            else if (pid < 0)
            {
                /* The fork failed.  */
                perror("fork");
                exit(1);
            }
            else
            {
                /* This is the parent process.  */
                p->pid = pid;
                if (get_shell_is_interactive())
                {
                    if (!j->pgid) j->pgid = pid;
                    setpgid(pid, j->pgid);
                }
            }
        }
        /* Clean up after pipes.  */
        if (infile != j->stdin)
            close(infile);
        if (outfile != j->stdout)
            close(outfile);
        infile = mypipe[0];
      }

    //format_job_info(j, "launched");

    if (!get_shell_is_interactive()) wait_for_job(j);
    else if (foreground) put_job_in_foreground(j, 0);
    else                 put_job_in_background(j, 0);
}

void put_job_in_foreground(job *j, int cont)
{
    /* Put the job into the foreground.  */
    tcsetpgrp(get_shell_terminal(), j->pgid);

    /* Send the job a continue signal, if necessary.  */
    if (cont)
    {
        tcsetattr(get_shell_terminal(), TCSADRAIN, &j->tmodes);
        if (kill(- j->pgid, SIGCONT) < 0) perror("kill (SIGCONT)");
    }

    /* Wait for it to report.  */
    wait_for_job(j);

    /* Put the shell back in the foreground.  */
    tcsetpgrp(get_shell_terminal(), get_shell_pgid());

    /* Restore the shell’s terminal modes.  */
    tcgetattr (get_shell_terminal(), &j->tmodes);
    tcsetattr (get_shell_terminal(), TCSADRAIN, get_shell_tmodes());
}

void put_job_in_background(job *j, int cont)
{
    /* Send the job a continue signal, if necessary.  */
    if (cont)
        if (kill(-j->pgid, SIGCONT) < 0)
            perror("kill (SIGCONT)");
}

int mark_process_status(pid_t pid, int status)
{
    job *j;
    process *p;

    if (pid > 0)
    {
        /* Update the record for the process.  */
        for (j = first_job; j; j = j->next)
            for (p = j->first_process; p; p = p->next)
                if (p->pid == pid)
                {
                    p->status = status;
                    if (WIFSTOPPED(status))
                        p->stopped = 1;
                    else
                    {
                        p->completed = 1;
                        if (WIFSIGNALED(status))
                            fprintf(stderr, "%d: Terminated by signal %d.\n",
                                     (int) pid, WTERMSIG(p->status));
                    }
                    return 0;
                }
        fprintf(stderr, "No child process %d.\n", pid);
        return -1;
    }
    else if (pid == 0 || errno == ECHILD)
        /* No processes ready to report.  */
        return -1;
    else {
        /* Other weird errors.  */
        perror ("waitpid");
        return -1;
    }
}

void update_status(void)
{
    int status;
    pid_t pid;

    do pid = waitpid(WAIT_ANY, &status, WUNTRACED|WNOHANG);
    while (!mark_process_status(pid, status));
}

void wait_for_job(job *j)
{
    int status;
    pid_t pid;

    do pid = waitpid(WAIT_ANY, &status, WUNTRACED);
    while (!mark_process_status(pid, status)
            && !job_is_stopped (j)
            && !job_is_completed (j));
}

void format_job_info(job *j, const char *status)
{
    fprintf(stderr, "%ld (%s): %s\n", (long)j->pgid, status, j->command);
}

void do_job_notification()
{
    job *j, *jlast, *jnext;

    /* Update status information for child processes.  */
    update_status();

    jlast = NULL;
    for (j = first_job; j; j = jnext)
    {
        jnext = j->next;

        /* If all processes have completed, tell the user the job has
            completed and delete it from the list of active jobs.  */
        if (job_is_completed (j))
        {
            format_job_info(j, "completed");
            if (jlast)
            jlast->next = jnext;
            else
            first_job = jnext;
            free_job(j);
        }

        /* Notify the user about stopped jobs,
            marking them so that we won’t do this more than once.  */
        else if (job_is_stopped(j) && !j->notified)
        {
            format_job_info(j, "stopped");
            j->notified = 1;
            jlast = j;
        }

        /* Don’t say anything about jobs that are still running.  */
        else jlast = j;
    }
}

void mark_job_as_running(job *j)
{
    process *p;

    for (p = j->first_process; p; p = p->next)
        p->stopped = 0;
    j->notified = 0;
}

void continue_job(job *j, int foreground)
{
    mark_job_as_running(j);
    if (foreground)
        put_job_in_foreground(j, 1);
    else
        put_job_in_background(j, 1);
}
