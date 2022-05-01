#ifndef UTIL_H_
#define UTIL_H_

/* Returns the basename of file from full path */
char* basename(char const* path);

/* Returns the path of file from full path */
char* dirname(char const* path);

/* Returns the current time in format hh:mm:ss */
void getTimeStr(char* timeString);

/* used for reloading */
void setGlobalArgv(char** argv);
char** getGlobalArgv();
void setLaunchDir();
char* getLaunchDir();

#endif // UTIL_H_
