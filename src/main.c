#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>

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

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <extension> <from> <to>\n", argv[0]);
        return 1;
    }

    DIR* from_dir = open_directory(argv[2]);
    
    return 0;
}
