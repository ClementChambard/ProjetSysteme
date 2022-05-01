#ifndef COMMAND_H_
#define COMMAND_H_

#include "util.h"
#include "job.h"

#include <ctype.h>

void try_parse_command(char* command);

void try_parse_job(char* jobstr);

void try_parse_process(char* process);

#endif // COMMAND_H_
