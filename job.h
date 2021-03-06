#ifndef JOB_H_
#define JOB_H_

#include <termios.h>
#include <stdio.h>
#include "process.h"

/* A job is a pipeline of processes.  */
typedef struct job
{
  struct job* next;           /* next active job */
  process* first_process;     /* list of processes in this job */
  pid_t pgid;                 /* process group ID */
  char notified;              /* true if user told about stopped job */
  struct termios tmodes;      /* saved terminal modes */
  int stdin, stdout, stderr;  /* standard i/o channels */
  char background;            /* is the job running in the background */
} job;

/* prints the job command from the argv of processes */
void print_job_command(job* j);
void fprint_job_command(job* j, FILE* f);

/* getter for global job variables */
job* get_first_job();

/* initialize job */
job* new_job();

/* add process to job */
process* add_process(job* j);

/* Free job j */
void free_job(job* j);

/* Find the active job with the indicated pgid.  */
job* find_job(pid_t pgid);

/* Return true if all processes in the job have stopped or completed.  */
int job_is_stopped(job* j);

/* Return true if all processes in the job have completed.  */
int job_is_completed(job* j);

void launch_job(job *j, int foreground);

/* Put job j in the foreground.  If cont is nonzero,
   restore the saved terminal modes and send the process group a
   SIGCONT signal to wake it up before we block.  */
void put_job_in_foreground(job *j, int cont);

/* Put a job in the background.  If the cont argument is true, send
   the process group a SIGCONT signal to wake it up.  */
void put_job_in_background(job *j, int cont);

/* Store the status of the process pid that was returned by waitpid.
   Return 0 if all went well, nonzero otherwise.  */
int mark_process_status(pid_t pid, int status);

/* Check for processes that have status information available,
   without blocking.  */
void update_status(void);

/* Check for processes that have status information available,
   blocking until all processes in the given job have reported.  */
void wait_for_job(job *j);

/* Format information about job status for the user to look at.  */
void format_job_info(job *j, const char *status);

/* Notify the user about stopped or terminated jobs.
   Delete terminated jobs from the active job list.  */
void do_job_notification();

/* Mark a stopped job J as being running again.  */
void mark_job_as_running(job *j);

/* Continue the job J.  */
void continue_job(job *j, int foreground);

#endif // JOB_H_
