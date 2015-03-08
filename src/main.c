#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include <gtk/gtk.h>

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

    rewinddir(dir);
    return files - num_files;
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

unsigned num_files_with_extension(DIR* dir, char* extension)
{
    char** files = get_files_in_directory(dir);
    unsigned num_files = num_files_in_directory(dir);

    unsigned num = 0;

    for (unsigned i = 0; i < num_files; ++i)
    {
        char* ext = get_extension_from_filename(files[i]);
        if (strcmp(ext, extension) == 0)
        {
            ++num;
        }
    }

    return num;
}

char** get_files_with_extension(DIR* dir, char* extension)
{
    char** files = get_files_in_directory(dir);
    unsigned num_files = num_files_in_directory(dir);
    unsigned num_files_with_ext = num_files_with_extension(dir, extension);

    char** files_with_extension = malloc(sizeof(char*) * num_files_with_ext);

    for (unsigned i = 0; i < num_files; ++i)
    {
        char* ext = get_extension_from_filename(files[i]);
        if (strcmp(ext, extension) == 0)
        {
            *files_with_extension++ = strdup(files[i]);
        }
    }

    for (unsigned i = 0; i < num_files; ++i)
    {
        free(files[i]);
    }
    free(files);

    return files_with_extension - num_files_with_ext;
}

void free_files_array(char** array, unsigned num)
{
    for (unsigned i = 0; i < num; ++i)
    {
        free(array[i]);
    }

    free(array);
}

void move_files(char* extension, char* src_dir, char* dest_dir)
{
    DIR* from_dir = open_directory(src_dir);
    char** files_to_move = get_files_with_extension(from_dir, extension);
    unsigned num_files_to_move = num_files_with_extension(from_dir, extension);

    for (unsigned i = 0; i < num_files_to_move; ++i)
    {
        char* filename = files_to_move[i];
        char old_path[strlen(filename) + strlen(src_dir) + 1];
        char new_path[strlen(filename) + strlen(dest_dir) + 1];
        strcpy(old_path, src_dir);
        strcat(old_path, "/");
        strcat(old_path, filename);
        
        strcpy(new_path, dest_dir);
        strcat(new_path, "/");
        strcat(new_path, filename);

        if (rename(old_path, new_path) == -1)
        {
            free_files_array(files_to_move, num_files_to_move);
            closedir(from_dir);
            error(1, errno, "Unable to move \"%s\" to \"%s\"\n", old_path, new_path);
        }
    }

    free_files_array(files_to_move, num_files_to_move);
    closedir(from_dir);
}

int main(int argc, char* argv[])
{
    gtk_init(&argc, &argv);

    if (argc == 1)
    {
        GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(window), "gmv");
        g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
        gtk_widget_show(window);

        GtkWidget *from_btn = gtk_button_new_with_label("From"),
                  *to_btn = gtk_button_new_with_label("To"),
                  *move_btn = gtk_button_new_with_label("Move"),
                  *from_entry = gtk_entry_new(),
                  *to_entry = gtk_entry_new();

        gtk_editable_set_editable(GTK_EDITABLE(from_entry), FALSE);
        gtk_editable_set_editable(GTK_EDITABLE(to_entry), FALSE);

        GtkWidget* grid = gtk_grid_new();
        gtk_container_add(GTK_CONTAINER(window), grid);
        gtk_grid_attach(GTK_GRID(grid), from_entry, 0, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), from_btn, 1, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), to_entry, 0, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), to_btn, 1, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), move_btn, 0, 2, 2, 1);

        GtkWidget* dialog = gtk_file_chooser_dialog_new("Select folder",
                GTK_WINDOW(window),
                GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                "Cancel", GTK_RESPONSE_CANCEL,
                "Select", GTK_RESPONSE_ACCEPT,
                NULL);

        gtk_widget_show_all(window);

        /* https://github.com/steshaw/gtk-examples/blob/master/ch04.button.edit.combo/listbox.c */

        gtk_main();
    }
    else if (argc == 4)
    {
        move_files(argv[1], argv[2], argv[3]);
    }
    else
    {
        printf("Usage:\nCommand line: %s <extension> <from> <to>\nGUI: %s\n", argv[0], argv[0]);
        return 1;
    }
}
