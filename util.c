#include "util.h"

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


char* basename(char const* path)
{
    char* s = strrchr(path, '/');
    if (!s)
        return strdup(path);
    else
        return strdup(s + 1);
}

char* dirname(char const* path)
{
    int l = strlen(path);
    char* s = malloc(l--);
    strcpy(s, path);
    while(s[l] != '/' && l >= 0) s[l--] = 0;
    return s;
}

void getTimeStr(char* timeString) {
    time_t secs = time(0);
    struct tm *local = localtime(&secs);
    sprintf(timeString, "%02d:%02d:%02d", local->tm_hour, local->tm_min, local->tm_sec);
}

char** argv;
char launchDir[100];
void setGlobalArgv(char** args) { argv = args; }
char** getGlobalArgv() { return argv; }
void setLaunchDir()
{
    getcwd(launchDir, 100);
}
char* getLaunchDir() { return launchDir; }


void getStringEscapeSequence(char* str)
{
    // replace occurrences of sequence "\033" with '\033'
    char* s = str;
    char* s2 = str;
    while (*s)
    {
        if (*s == '\\' && *(s + 1) == '\\')
        {
            s++;
            *(s2++) = *(s++);
        }
        else if (*s == '\\' && *(s + 1) == 'e')
        {
            s++;
            s++;
            *(s2++) = 27;
        }
        else
        {
            *(s2++) = *(s++);
        }
    }
    *s2 = 0;
}

void replaceHomeWithTilde(char* str)
{
    // get home directory
    char* home = getenv("HOME");
    if (!home) return;
    // replace occurrences of home directory with tilde
    if (strncmp(home, str, strlen(home)) == 0)
    {
        strcpy(str+1, str+strlen(home));
        str[0] = '~';
    }
}