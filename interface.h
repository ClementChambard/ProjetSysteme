#ifndef INTERFACE_H_
#define INTERFACE_H_

/* Prompts for a command */
void prompt();

/* Returns the typed command */
char* getAnwser();

/* Initialize interface variables */
void init_interface();

/* Free interface variables */
void free_interface();

#endif // INTERFACE_H_
