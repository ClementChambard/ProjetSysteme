#ifndef COMMAND_H_
#define COMMAND_H_

#include "util.h"
#include "job.h"

#include <ctype.h>

/**
 * try to launch jobs based on command
 */
void try_parse_command(char* command);

/**
 * try to launch one job based on jobstr
 */
void try_parse_job(char* jobstr);

/**
 * try to launch a process based on string process
 */
void try_parse_process(char* process);

#endif // COMMAND_H_
