#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct alias
{
    char *name;
    char** argv;
    struct alias *next;
    
} alias_t;

alias_t* first_alias = NULL;

char** find_alias(char* name)
{
    
    alias_t* a = first_alias;
    while (a)
    {
        if (!strcmp(a->name, name)){ return a->argv;}
        a = a->next;
    }
    return NULL;
}

void free_alias(alias_t* a)
{
    if (!a) return;
    free(a->name);
    char** av = a->argv;
    while (*av) free(*(av++));
    free(a->argv);
    free_alias(a->next);
    free(a);
}

void free_aliases() { free_alias(first_alias); }

void add_alias(char *name, char *value){
    
    alias_t *first = first_alias;
    first_alias = malloc(sizeof(alias_t));
    first_alias->next = first;
    first_alias->name = strdup(name);

    int cnt = 0;
    char* value_copy = strdup(value);
    char* token = strtok(value_copy, " ");
    while (token)
    {
        // /*DEBUG*/ printf("%s\n", token);
        cnt++;
        token = strtok(NULL, " ");
    }
    free(value_copy);
    first_alias->argv = malloc((cnt+1) * sizeof(char*));
    cnt = 0;
    token = strtok(value, " ");
    while (token)
    {
        
        first_alias->argv[cnt] = strdup(token);
        cnt++;
        token = strtok(NULL, " ");
    }
    first_alias->argv[cnt] = NULL;

    //check argv[0] for recursive alias
    char** aliasargv = find_alias(first_alias->argv[0]);
    if (aliasargv)
    {
        //printf("alias: %s\n", first_alias->argv[0]);
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
            newargv[i+aliasargc-1] = first_alias->argv[i];
            i++;
        }
        newargv[i+aliasargc-1] = NULL;
        free(first_alias->argv[0]);
        free(first_alias->argv);
        first_alias->argv = newargv;
    }
    
}
