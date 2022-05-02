#include "builtin.h"
#include "util.h"

#include <pwd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void cd(char** args)
{
    if (args[0] && args[1])
    {
        printf("shell: cd: too many arguments\n");
        return;
    }

    char* dir = args[0];
    if (!dir) dir = "";

    struct passwd* pw = getpwuid(getuid());
    char* homedir = pw->pw_dir;

    if (dir[0] == 0 || (dir[0] == '~' && dir[1] == 0)) dir = homedir;
    int res = chdir(dir);
    if (res < 0)
    printf("shell: cd: %s: No such directory\n", dir);
}

void reload(char** args)
{
    if (args[0])
    {
        printf("shell: reload: too many arguments\n");
        return;
    }
    int res = chdir(getLaunchDir());
    if (res < 0)
    printf("shell: reload: might fail due to the original launch directory being unreachable\n");
    execvp(getGlobalArgv()[0], getGlobalArgv());
    exit(0);
}

void recompile(char** args)
{
    if (args[0])
    {
        printf("shell: recompile: too many arguments\n");
        return;
    }

    pid_t pid = fork();
    if (pid == 0)
    {
        /* This is the child process.  */
        char* path = dirname(getGlobalArgv()[0]);
        int res = chdir(getLaunchDir());
        if (res < 0)
        printf("shell: recompile: might fail due to the original launch directory being unreachable\n");
        res = chdir(path);
        if (res < 0)
        printf("shell: recompile: might fail due to the original launch directory being unreachable\n");
        char* argv[] = {"./compile", NULL};
        execvp(argv[0], argv);
        exit(0);
    }
    else wait(NULL);
}
