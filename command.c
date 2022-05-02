#include "command.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int jobBG = 0;

void try_parse_command(char* command)
{
    // /* DEBUG */ printf("command : %s\n", command);
    static char jobStr[300];
    int i = 0;
    int bs = 0;
    while (*command)
    {
        if (*command == '&' || *command == ';')
        {
            if (bs) { bs = 0; }
            else if (*command == ';' || command[1] == '&')
            {
                jobBG = 0;
                command += 2;
                jobStr[i++] = 0;
                try_parse_job(jobStr);
                i = 0;
                continue;
            }
            else
            {
                jobBG = 1;
                command++;
                jobStr[i++] = 0;
                try_parse_job(jobStr);
                i = 0;
                continue;
            }
        }

        if (bs) { bs = 0; jobStr[i++] = '\\'; }
        if (*command == '\\') bs = 1;

        jobStr[i++] = *(command++);
    }
    jobStr[i++] = 0;
    jobBG = 0;
    try_parse_job(jobStr);
}

job* currentJob = NULL;

void try_parse_job(char* jobstr)
{

    // /* DEBUG */ printf("job : %s\n", jobstr);
    currentJob = new_job();
    currentJob->background = jobBG;
    static char procStr[300];
    int i = 0;
    while (*jobstr)
    {
        if (*jobstr == '|')
        {
            procStr[i++] = 0;
            try_parse_process(procStr);
            i = 0;
            jobstr++;
            continue;
        }
        procStr[i++] = *(jobstr++);
    }
    procStr[i++] = 0;
    try_parse_process(procStr);
    launch_job(currentJob, !jobBG);
    do_job_notification();
}

void try_parse_process(char* procstr)
{
    // TODO: redirect, backslashes, alias
    int cnt = 1;
    char* ps = procstr;
    int space = 1;
    while (*ps)
    {
        if (space && !isspace(*ps))
        {
            space = 0;
            cnt++;
        }
        else if (isspace(*ps))
        {
            space = 1;
        }
        ps++;
    }
    if (cnt == 1) return; // Error ?
    // /* DEBUG */ printf("process : %s:", procstr);
    // /* DEBUG */ printf(" argc = %d\n", cnt-1);
    char** argv = malloc(cnt * sizeof(char*));
    int i = 0, j = 0;
    space = 1;
    argv[cnt-1] = NULL;
    if (cnt > 1) argv[0] = malloc(100);
    while (*procstr)
    {
        if (space && !isspace(*procstr))
        {
            space = 0;
        }
        else if (isspace(*procstr))
            if (!space)
            {
                space = 1;
                argv[j++][i] = 0;
                // /* DEBUG */ printf("arg%d: %s\n", j-1, argv[j-1]);
                i = 0;
                if (j < cnt-1) argv[j] = malloc(100);
                else break;
            }
        if (!space)
        {
            argv[j][i++] = *procstr;
        }
        procstr++;
    }
    if (j < cnt-1)
    {
        argv[j][i] = 0;
        // /* DEBUG */ printf("arg%d: %s\n", j, argv[j]);
    }
    process* p = add_process(currentJob);
    p->argv = argv;
}
