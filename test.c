#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

void print_menu(char *path)
{
    printf("Menu for %s:\n", path);
    printf("1. Read the file\n");
    printf("2. Write to the file\n");
    printf("3. Execute the file\n");
    printf("4. List the directory contents\n");
    printf("5. Print the symbolic link\n");
    printf("6. Exit\n");
}

void read_file(char *path)
{
    FILE *file = fopen(path, "r");

    {
        perror("fopen");
        return;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        printf("%s", buffer);
    }

    fclose(file);
}

void write_file(char *path)
{
    FILE *file = fopen(path, "w");
    if (file == NULL)
    {
        perror("fopen");
        return;
    }

    printf("Enter text to write to %s (press Ctrl-D to finish):\n", path);

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), stdin) != NULL)
    {
        fputs(buffer, file);
    }

    fclose(file);
}

void execute_file(char *path)
{
    if (access(path, X_OK) == -1)
    {
        perror("access");
        return;
    }

    system(path);
}

void list_directory(char *path)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
}

void print_symbolic_link(char *path)
{
    char target[256];
    ssize_t len = readlink(path, target, sizeof(target) - 1);
    if (len == -1)
    {
        perror("readlink");
        return;
    }

    target[len] = '\0';
    printf("The target of %s is %s\n", path, target);
}

int main(int argc, char *argv[])
{
    int i;
    struct stat st;

    for (i = 1; i < argc; i++)
    {
        if (lstat(argv[i], &st) == -1)
        {
            perror("lstat");
            continue;
        }

        if (S_ISREG(st.st_mode))
        {
            print_menu(argv[i]);

            int choice;
            do
            {
                printf("Enter your choice (1-6): ");
                scanf("%d", &choice);
            } while (choice < 1 || choice > 6);

            switch (choice)
            {
            case 1:
                printf("Reading file %s\n", argv[i]);
                read_file(argv[i]);
                break;
            case 2:
                printf("Writing to file %s\n", argv[i]);
                write_file(argv[i]);
                break;
            case 3:
                printf("Executing file %s\n", argv[i]);
                execute_file(argv[i]);
                break;
            case 4:
                printf("Listing directory %s:\n", argv[i]);
                list_directory(argv[i]);
                break;
            case 5:
                printf("Printing symbolic link target for %s:\n", argv[i]);
                print_symbolic_link(argv[i]);
                break;
            case 6:
                printf("Exiting menu for %s\n", argv[i]);
                break;
            default:
                printf("Invalid choice\n");
                break;
            }
        }
        else if (S_ISDIR(st.st_mode))
        {
            printf("Listing directory %s:\n", argv[i]);
            list_directory(argv[i]);
        }
        else if (S_ISLNK(st.st_mode))
        {
            printf("%s is a symbolic link\n", argv[i]);
            print_symbolic_link(argv[i]);
        }
        else
        {
            printf("%s is not a regular file, directory, or symbolic link\n", argv[i]);
        }
    }

    return 0;
}