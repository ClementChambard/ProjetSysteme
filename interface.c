#include "interface.h"
#include "util.h"

#include "job.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* PS0ex = "";
char* PS1ex = "[\033[34m&t\033[0m] [\033[35m&u@&h &W\033[0m]$ ";
char* PS2ex = "";
char  PS0str[200];
char  PS1str[200];
char  PS2str[200];
int   PS0refresh = 0;
int   PS1refresh = 0;
int   PS2refresh = 0;

char* Anwser = NULL;

void refreshPromptStr(char* ex, char* str, int* hasToBeRefreshed)
{
    int refresh = 0;
    static char usr[50];
    static char host[50];
    static char cwd[100];
    static char tim[9];
    int i = 0, iStr = 0;
    for (char c = ex[i]; c != 0; c = ex[++i])
        if (c == '&')
        {
            c = ex[++i];
            if (c == 'u')
            {
                getlogin_r(usr, 50);
                char* u = usr;
                while (*u) str[iStr++] = *(u++);
                refresh = 1;
            }
            else if (c == 'h')
            {
                gethostname(host, 50);
                char* h = host;
                while (*h) str[iStr++] = *(h++);
                refresh = 1;
            }
            else if (c == 'W' || c == 'w')
            {
                getcwd(cwd, 100);
                char* cwdstr = cwd;
                if (c == 'W') { cwdstr = basename(cwd); }
                char* d = cwdstr;
                while (*d) str[iStr++] = *(d++);
                if (c == 'W') free(cwdstr);
                refresh = 1;
            }
            else if (c == 'A' || c == 't')
            {
                getTimeStr(tim);
                int lim = 8; if (c == 'A') lim = 5;
                for (int i = 0; i < lim; i++) str[iStr++] = tim[i];
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
    if (Anwser != NULL) free(Anwser);
    read_size = getline(&Anwser, &buf_size, stdin);
    Anwser[read_size-1] = 0;
    if (PS0refresh) refreshPromptStr(PS0ex, PS0str, NULL);
    if (Anwser[0] != 0) printf("%s", PS0str);
    // /* DEBUG */ printf("read %zd bytes in %zd bytes buffer '%s'\n", read_size, buf_size, Anwser);
}

char* getAnwser() { return Anwser; }

void init_interface()
{
    refreshPromptStr(PS0ex, PS0str, &PS0refresh);
    refreshPromptStr(PS1ex, PS1str, &PS1refresh);
    refreshPromptStr(PS2ex, PS2str, &PS2refresh);
}
