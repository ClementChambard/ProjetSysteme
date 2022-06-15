#ifndef ALIAS_H
#define ALIAS_H

/**
 * Free all alias structs
 */
void free_aliases();

/**
 * Adds an alias
 * alias name=value
 */
void add_alias(char *name, char *value);

/**
 * Returns the appropriate argv for an alias name
 * returns NULL if not found
 */
char** find_alias(char *name);

#endif // ALIAS_H
