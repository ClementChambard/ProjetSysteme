#ifndef     CONFIG_H_
#define     CONFIG_H_

/**
 * Getters for configurated variables
 */
char* getPS0ex();
char* getPS1ex();
char* getPS2ex();

/* free the configurated variables if needed */
void free_config();

/* Loads the configurated variables from a config file */
void load_config(char* filename);

#endif // CONFIG_H_
