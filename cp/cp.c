#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

#include "../util.h"

char* buffer;
size_t bloc_size;

// returns the name of the file 'path' without directory

// returns the path of the file 'file' if the destination 'path' is a directory
char* correctPath(char const* file, char* path)
{
    char* base = basename(file);
    char* d = strdup(path);
    if (d[strlen(d)-1] != '/') d = strcat(d, "/");
    d = strcat(d, base);
    free(base);
    return d;
}

// copies a single file 'inputFileName' as 'outputFileName'
void copyFile(char* inputFileName, char* outputFileName)
{
    struct stat statbuf = {};
    stat(outputFileName, &statbuf);
    char freeOutputFile = 0;
    if (!S_ISREG(statbuf.st_mode) && !access(outputFileName, F_OK))
    {
        outputFileName = correctPath(inputFileName, outputFileName);
        freeOutputFile = 1;
    }

    int inputFile = open(inputFileName, O_RDONLY);
    int outputFile = open(outputFileName, O_WRONLY | O_CREAT);

    ssize_t read_size = 4096;

    while (read_size >= bloc_size)
    {
        read_size = read(inputFile, buffer, bloc_size);
        write(outputFile, buffer, read_size);
    }

    fstat(inputFile, &statbuf);
    fchmod(outputFile, statbuf.st_mode);

    close(inputFile);
    close(outputFile);

    if (freeOutputFile) free(outputFileName);
}

// copies recursively 'inputFileName' as 'outputFileName'
void copy(char* inputFileName, char* outputFileName)
{
    struct stat statbuf = {};
    stat(inputFileName, &statbuf);
    if (access(inputFileName, F_OK)) return;
    if (S_ISREG(statbuf.st_mode))
    {
        copyFile(inputFileName, outputFileName);
        return;
    }

    char freeOutputFile = 0;
    if (!access(outputFileName, F_OK))
    {
        outputFileName = correctPath(inputFileName, outputFileName);
        freeOutputFile = 1;
    }

    mkdir(outputFileName, statbuf.st_mode);
    struct dirent* dp;
    DIR* dir = opendir(inputFileName);

    while ((dp = readdir(dir)))
    {
        if (dp->d_name[0] == '.')
        {
            if (dp->d_name[1] == 0) continue;
            if (dp->d_name[1] == '.' && dp->d_name[2] == 0) continue;
        }
        char* in = correctPath(dp->d_name, inputFileName);
        char* out = correctPath(dp->d_name, outputFileName);

        copy(in, out);

        free(in);
        free(out);
    }

    closedir(dir);

    if (freeOutputFile) free(outputFileName);
}

void cp(int argc, char** argv)
{
    if (argc != 3)
    {
        printf("wrong number of arguments\n      cp <input> <output>\n");
        return ;
    }

    char* inputFileName = argv[1];
    char* outputFileName = argv[2];

    buffer = malloc(4096);
    bloc_size = 4096;
    copy(inputFileName, outputFileName);

    free(buffer);

    return ;
}
