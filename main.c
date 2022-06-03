#include "interface.h"
#include "util.h"
#include "command.h"
#include "shell.h"
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    init_shell();
    init_interface();
    setGlobalArgv(argv);
    setLaunchDir();
    while (1)
    {
        prompt();

        do_job_notification();

        char* a = getAnwser();
        if (!*a) continue;
        if (!strcmp(getAnwser(), "exit")) break;

        try_parse_command(a);
    }
    free(getAnwser());
    job* j = get_first_job();
    job* j1;
    while(j) {j1 = j; j = j->next; free_job(j1);}
    return 0;
}
