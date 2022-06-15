#include "command.h"
#include "util.h"
#include "alias.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int jobBG = 0;

int infile = STDIN_FILENO;
int outfile = STDOUT_FILENO;
int errfile = STDERR_FILENO;

void try_parse_command(char* command)
{
    // /* DEBUG */ printf("command : %s\n", command);
    infile = STDIN_FILENO;
    outfile = STDOUT_FILENO;
    errfile = STDERR_FILENO;
    static char jobStr[300];
    static char redirectFile[100];
    int i = 0;
    int ri = 0;
    int bs = 0;
    int rs = 0;
    int str = 0;
    int redirect = 0;
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

        if (*command == '>') { redirect = 1; command++; continue; }
        if (*command == '<') { redirect = 2; command++; continue; }

        if (redirect)
        {
            if (!rs && !isspace(*command)) rs = 1;
            if (rs)
            {
                if (*command == '"' && !str) str = 1;
                else if ((*command == '"' && str) || (!str && (isspace(*command) || *command == 0)))
                {
                    redirectFile[ri++] = 0;
                    ri = 0;
                    rs = 0;
                    str = 0;
                    if (redirect == 1) outfile = open(redirectFile, O_WRONLY | O_CREAT, 0644);
                    else if (redirect == 2) infile = open(redirectFile, O_RDONLY);
                    redirect = 0;
                }
                else redirectFile[ri++] = *command;
            }
            command++;
            continue;
        }

        if (bs) { bs = 0; jobStr[i++] = '\\'; }
        if (*command == '\\') bs = 1;

        jobStr[i++] = *(command++);
    }
    if (redirect)
    {
        redirectFile[ri++] = 0;
        if (redirect == 1) outfile = open(redirectFile, O_WRONLY | O_CREAT, 0644);
        else if (redirect == 2) infile = open(redirectFile, O_RDONLY);
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
    currentJob->stdin = infile;
    currentJob->stdout = outfile;
    currentJob->stderr = errfile;
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
    // check argv[0] for alias
    char** aliasargv = find_alias(argv[0]);

    if (aliasargv)
    {
        //printf("alias: %s\n", argv[0]);
        // get size of aliasargv
        int aliasargc = 0;
        while (aliasargv[aliasargc]) aliasargc++;
        char** newargv;
        // fill newargv with aliasargv and argv
        newargv = malloc((cnt+aliasargc) * sizeof(char*));
        int i = 0;
        while (i < aliasargc)
        {
            //printf("%s\n", aliasargv[i]);
            newargv[i] = strdup(aliasargv[i]);
            i++;
        }
        i = 1;
        while (i < cnt)
        {
            newargv[i+aliasargc-1] = argv[i];
            i++;
        }
        newargv[i+aliasargc-1] = NULL;

        p->argv = newargv;
        free(argv[0]);
        free(argv);
    }
    else
    {
        p->argv = argv;
    }
}
