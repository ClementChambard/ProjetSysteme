#include "interface.h"
#include "util.h"
#include "command.h"
#include "shell.h"
#include <string.h>

int main(int argc, char** argv) {
    init_shell();
    initInterface();
    setGlobalArgv(argv);
    setLaunchDir();
    while (1)
    {
        prompt();

        char* a = getAnwser();
        if (!*a) continue;
        if (!strcmp(getAnwser(), "exit")) break;

        try_parse_command(a);
    }
    freeInterface();
    return 0;
}
