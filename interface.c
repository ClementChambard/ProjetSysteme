#include "interface.h"
#include "util.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* PS0ex = "";
char* PS0str = NULL;
int   PS0refresh = 0;
char* PS1ex = "[\033[34m&t\033[0m] [\033[35m&u@&h &W\033[0m]$ ";
char* PS1str = NULL;
int   PS1refresh = 0;
char* PS2ex = "";
char* PS2str = NULL;
int   PS2refresh = 0;

char* Anwser = NULL;

void refreshPromptStr(char* ex, char* str, int* hasToBeRefreshed)
{
    int refresh = 0;
    int i = 0, iStr = 0;
    for (char c = ex[i]; c != 0; c = ex[++i])
        if (c == '&')
        {
            c = ex[++i];
            if (c == 'u')
            {
                char* usr = malloc(50); getlogin_r(usr, 50);
                char* u = usr;
                while (*u) str[iStr++] = *(u++);
                free(usr);
                refresh = 1;
            }
            else if (c == 'h')
            {
                char* host = malloc(50); gethostname(host, 50);
                char* h = host;
                while (*h) str[iStr++] = *(h++);
                free(host);
                refresh = 1;
            }
            else if (c == 'W' || c == 'w')
            {
                char* cwd = malloc(100); getcwd(cwd, 100);
                char* d = cwd;
                if (c == 'W') d = basename(cwd);
                while (*d) str[iStr++] = *(d++);
                free(cwd);
                refresh = 1;
            }
            else if (c == 'A' || c == 't')
            {
                char* tim = malloc(9); getTimeStr(tim);
                int lim = 8; if (c == 'A') lim = 5;
                for (int i = 0; i < lim; i++) str[iStr++] = tim[i];
                free(tim);
                refresh = 1;
            }
            else
            {
                str[iStr++] = '&';
                str[iStr++] = c;
            }
        }
        else str[iStr++] = c;
    str[iStr] = 0;
    if (hasToBeRefreshed != NULL) *hasToBeRefreshed = refresh;
}

void prompt()
{
    if (PS1refresh) refreshPromptStr(PS1ex, PS1str, NULL);
    if (PS2refresh) refreshPromptStr(PS2ex, PS2str, NULL);
    printf("%s", PS1str);
    size_t buf_size = 0;
    ssize_t read_size;
    read_size = getline(&Anwser, &buf_size, stdin);
    Anwser[read_size-1] = 0;
    if (PS0refresh) refreshPromptStr(PS0ex, PS0str, NULL);
    printf("%s", PS0str);
    // /* DEBUG */ printf("read %zd bytes in %zd bytes buffer '%s'\n", read_size, buf_size, Anwser);
}

char* getAnwser() { return Anwser; }

void initInterface()
{
    PS0str = malloc(200); refreshPromptStr(PS0ex, PS0str, &PS0refresh);
    PS1str = malloc(200); refreshPromptStr(PS1ex, PS1str, &PS1refresh);
    PS2str = malloc(200); refreshPromptStr(PS2ex, PS2str, &PS2refresh);
    Anwser = malloc(400);
}

void freeInterface()
{
    free(PS0str);
    free(PS1str);
    free(PS2str);
}

