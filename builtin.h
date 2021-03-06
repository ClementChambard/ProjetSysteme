#ifndef BUILTIN_H_
#define BUILTIN_H_

/* Change the current working directory */
void cd(char** args);

/* reload the shell from the shell if it has been recompiled */
void reload(char** args);

/* recompile the shell from the shell */
void recompile(char** args);

/* adds an alias to the alias list */
void alias(char** args);

/* show the list of running jobs */
void jobs(char** args);

/* kill a job */
void kill_builtin(char** args);

#endif // BUILTIN_H_
