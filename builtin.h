#ifndef BUILTIN_H_
#define BUILTIN_H_

/* Change the current working directory */
void cd(char** args);

/* reload the shell from the shell if it has been recompiled */
void reload(char** args);

/* recompile the shell from the shell */
void recompile(char** args);

#endif // BUILTIN_H_
