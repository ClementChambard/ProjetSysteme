#include "command.h"

#include <stdlib.h>
#include <stdio.h>

void try_parse_command(char* command)
{
    // /* DEBUG */ printf("command : %s\n", command);
    static char jobStr[300];
    int i = 0;
    int sawAnd = 0;
    while (*command)
    {
        if (*command == '&')
        {
            if (sawAnd)
            {
                jobStr[i++] = 0;
                try_parse_job(jobStr);
                sawAnd = 0;
                i = 0;
            }
            else sawAnd = 1;
            command++;
            continue;
        }
        else if (sawAnd)
        {
            sawAnd = 0;
            jobStr[i++] = '&';
        }
        jobStr[i++] = *(command++);
    }
    jobStr[i++] = 0;
    try_parse_job(jobStr);
}

job* currentJob = NULL;

void try_parse_job(char* jobstr)
{
    // /* DEBUG */ printf("job : %s\n", jobstr);
    currentJob = new_job();
    currentJob->command = jobstr;
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
    launch_job(currentJob, 1);
}

void try_parse_process(char* procstr)
{
    // /* DEBUG */ printf("process : %s:", procstr);
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
