#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define MAX_FILENAME_LENGTH 256

DIR* open_directory(const char* dir_name)
{
    DIR* dir = opendir(dir_name);

    if (dir == NULL)
    {
        error(1, errno, "An error occured when trying to open the directory \"%s\"", dir_name);
    }

    return dir;
}

unsigned num_files_in_directory(DIR* dir)
{
    unsigned num_files = 0;

    struct dirent* file;
    while ((file = readdir(dir)) != NULL)
    {
        ++num_files;
    }

    rewinddir(dir);
    return num_files;
}

char* strdup(const char* str)
{
    char* duped = malloc(sizeof(char) * (strlen(str) + 1));
    strcpy(duped, str);
    return duped;
}

char** get_files_in_directory(DIR* dir)
{
    unsigned num_files = num_files_in_directory(dir);
    char** files = malloc(sizeof(char*) * num_files);

    struct dirent* file;
    while ((file = readdir(dir)) != NULL)
    {
        *files++ = strdup(file->d_name);
    }

    return files - num_files;
}

void list_files_in_directory(DIR* dir)
{
    unsigned num_files = num_files_in_directory(dir);
    char** files = get_files_in_directory(dir);
    for (unsigned i = 0; i < num_files; ++i)
    {
        puts(files[i]);
        free(files[i]);
    }

    free(files);
}

char* get_extension_from_filename(const char* filename)
{
    char* last_dot = strrchr(filename, '.');

    if (last_dot == NULL || *(last_dot + 1) == '\0')
    {
        return "";
    }
    else
    {
        unsigned extension_length = strlen(filename) - (last_dot - filename) - 1; /* -1 so we don't count the dot itself */
        char* extension = malloc(sizeof(char) * (extension_length + 1));
        strncpy(extension, last_dot + 1, extension_length + 1); /* extension_length + 1 so we copy the null character */

        return extension;
    }
}

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <extension> <from> <to>\n", argv[0]);
        return 1;
    }

    DIR* from_dir = open_directory(argv[2]);
    list_files_in_directory(from_dir);
    
    return 0;
}
