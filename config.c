#include "config.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "alias.h"


char* PS0ex = "";
char* PS1ex = "\033[01;32m&u@&h\033[0m:\033[01;34m&w\033[0m$ "; 
char* PS2ex = "";


char* getPS0ex() { return PS0ex; }
char* getPS1ex() { return PS1ex; }
char* getPS2ex() { return PS2ex; }

void load_config(char* filename){
    //load file
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        printf("Error: config file not found\n");
        return;
    }
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, f)) != -1) {
        //printf("Retrieved line of length %zu :\n", read);
        //printf("%s", line);
        line[read-1] = 0;
        if (strncmp(line, "PS0=", 4) == 0) {
            PS0ex = strdup(line + 4);
            getStringEscapeSequence(PS0ex);

        }
        else if (strncmp(line, "PS1=", 4) == 0) {
            PS1ex = strdup(line + 4);
            getStringEscapeSequence(PS1ex);
                
        }
        else if (strncmp(line, "PS2=", 4) == 0) {
            PS2ex = strdup(line + 4);
            getStringEscapeSequence(PS2ex);
        }
        else if (strncmp(line, "alias ", 6) == 0) {
            char* alias = line + 6;
            char* cmd = strchr(alias, '=');
            if (cmd == NULL) {
                printf("Error: alias syntax error\n");
                continue;
            }
            *cmd = 0;
            cmd++;
            add_alias(alias, cmd);
        }
    }
    fclose(f);
}