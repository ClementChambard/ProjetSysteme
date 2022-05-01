#ifndef PROCESS_H_
#define PROCESS_H_

#include <termios.h>

/* A process is a single process.  */
typedef struct process
{
  struct process *next;       /* next process in pipeline */
  char** argv;                /* for exec */
  pid_t pid;                  /* process ID */
  char completed;             /* true if process has completed */
  char stopped;               /* true if process has stopped */
  int status;                 /* reported status value */
} process;

void free_process(process* p);

int check_builtin_process(char** argv);

void launch_process(process *p, pid_t pgid, int infile, int outfile, int errfile, int foreground);

#endif // PROCESS_H_
