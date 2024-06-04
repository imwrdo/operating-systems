#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <time.h>
#include <libgen.h>
#include <sys/types.h>
#include <grp.h>

void print_long(char *dir_arg, struct dirent *dir_entry, int flag_size);
void flag_handler(char *dir_arg, struct dirent *dir_entry, int flag_all, int flag_long, int flag_size);
void print_args(char *dir_arg, char *file, int flag_all, int flag_long, int flag_file, int flag_reverse, int flag_size, int flag_recursive);

int main(int argc, char *argv[]) {
    int flag_long = 0;
    int flag_all = 0;
    int flag_file = 0;
    int flag_reverse = 0;
    int flag_size = 0;
    int flag_recursive = 0;

    int opt;
    while ((opt = getopt(argc, argv, "alrsR")) != -1) {
        switch (opt) {
            case 'l':
                flag_long = 1;
                break;
            case 'a':
                flag_all = 1;
                break;
            case 'r':
                flag_reverse = 1;
                break;
            case 's':
                flag_size = 1;
                break;
            case 'R':
                flag_recursive = 1;
                break;
            default:
                fprintf(stderr, "myls: supports -l, -a, -r, -s, and -R options\n");
                exit(EXIT_FAILURE);
        }
    }

    if (optind == argc) {
        print_args(".", "NULL", flag_all, flag_long, flag_file, flag_reverse, flag_size, flag_recursive);
        if (flag_long == 0) {
            printf("\n");
        }
    } else {
        while (optind < argc) {
            struct stat argbuf;
            char *arg = argv[optind];
            if ((stat(arg, &argbuf)) == -1) {
                printf("myls: cannot access '%s': No such file or directory\n", argv[optind]);
            } else {
                if (S_ISREG(argbuf.st_mode)) {
                    flag_file = 1;
                    print_args(".", arg, flag_all, flag_long, flag_file, flag_reverse, flag_size, flag_recursive);
                }
                if (S_ISDIR(argbuf.st_mode)) {
                    printf("%s:\n", arg);
                    print_args(arg, "NULL", flag_all, flag_long, flag_file, flag_reverse, flag_size, flag_recursive);
                }
                flag_file = 0;
                if (optind < argc - 1) {
                    printf("\n");
                }
                if (flag_long == 0) {
                    printf("\n");
                }
            }
            optind++;
        }
    }
}

void print_long(char *dir_arg, struct dirent *dir_entry, int flag_size) {
    struct stat statbuf;
    char fp[PATH_MAX];
    sprintf(fp, "%s/%s", dir_arg, dir_entry->d_name);
    if (stat(fp, &statbuf) == -1) {
        perror("stat");
        return;
    }

    printf((S_ISDIR(statbuf.st_mode)) ? "d" : "-");
    printf((statbuf.st_mode & S_IRUSR) ? "r" : "-");
    printf((statbuf.st_mode & S_IWUSR) ? "w" : "-");
    printf((statbuf.st_mode & S_IXUSR) ? "x" : "-");
    printf((statbuf.st_mode & S_IRGRP) ? "r" : "-");
    printf((statbuf.st_mode & S_IWGRP) ? "w" : "-");
    printf((statbuf.st_mode & S_IXGRP) ? "x" : "-");
    printf((statbuf.st_mode & S_IROTH) ? "r" : "-");
    printf((statbuf.st_mode & S_IWOTH) ? "w" : "-");
    printf((statbuf.st_mode & S_IXOTH) ? "x " : "- ");
    printf("%li ", statbuf.st_nlink);

    struct passwd *pw;
    struct group *gid;
    pw = getpwuid(statbuf.st_uid);
    if (pw == NULL) {
        perror("getpwuid");
        printf("%d ", statbuf.st_uid);
    } else {
        printf("%s ", pw->pw_name);
    }
    gid = getgrgid(statbuf.st_gid);
    if (gid == NULL) {
        perror("getpwuid");
        printf("%d ", statbuf.st_gid);
    } else {
        printf("%s ", gid->gr_name);
    }

    if (flag_size == 0) {
        printf("%5ld ", statbuf.st_size);
    } else {
        printf("%5ld ", statbuf.st_blocks / 2);
    }

    struct tm *tmp;
    char outstr[200];
    time_t t = statbuf.st_mtime;
    tmp = localtime(&t);
    if (tmp == NULL) {
        perror("localtime");
        exit(EXIT_FAILURE);
    }
    strftime(outstr, sizeof(outstr), "%b %d %R", tmp);
    printf("%s ", outstr);

    printf("%s\n", dir_entry->d_name);
}

void flag_handler(char *dir_arg, struct dirent *dir_entry, int flag_all, int flag_long, int flag_size) {
    if (flag_all == 0) {
        if ((dir_entry->d_name[0] == '.')) {
            return;
        }
    }
    if (flag_long == 0 && flag_size == 0) {
        printf("%s ", dir_entry->d_name);
    } else if (flag_long == 1 && flag_size == 0) {
        print_long(dir_arg, dir_entry, flag_size);
    } else if (flag_long == 0 && flag_size == 1) {
        struct stat statbuf;
        char fp[PATH_MAX];
        sprintf(fp, "%s/%s", dir_arg, dir_entry->d_name);
        if (stat(fp, &statbuf) == -1) {
            perror("stat");
            return;
        }
        printf("%ld ", statbuf.st_blocks);
        printf("%s ", dir_entry->d_name);
    } else {
        print_long(dir_arg, dir_entry, flag_size);
    }
}

void print_args(char *dir_arg, char *file, int flag_all, int flag_long, int flag_file, int flag_reverse, int flag_size, int flag_recursive) {
    DIR *dir = opendir(dir_arg);
    if (dir == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    int count_max = 1024;
    struct dirent *entries_alloc;
    struct dirent *entries[count_max];
    int count = 0;

    struct dirent *dir_entry;
    errno = 0;
    while ((dir_entry = readdir(dir)) != NULL) {
        entries_alloc = malloc(sizeof(struct dirent));
        if (entries_alloc == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        memcpy(entries_alloc, dir_entry, sizeof(struct dirent));
        if (flag_file == 1) {
            if (strcmp(dir_entry->d_name, file) == 0) {
                entries[count++] = entries_alloc;
            } else {
                free(entries_alloc);
            }
        } else {
            entries[count++] = entries_alloc;
        }
    }
    if ((dir_entry == NULL) && (errno != 0)) {
        perror("readdir");
        exit(EXIT_FAILURE);
    }

    closedir(dir);

    if (flag_reverse) {
        for (int i = count - 1; i >= 0; i--) {
            flag_handler(dir_arg, entries[i], flag_all, flag_long, flag_size);
            free(entries[i]);
        }
    } else {
        for (int i = 0; i < count; i++) {
            flag_handler(dir_arg, entries[i], flag_all, flag_long, flag_size);
            free(entries[i]);
        }
    }

    if (flag_recursive) {
        for (int i = 0; i < count; i++) {
            char path[PATH_MAX];
            snprintf(path, sizeof(path), "%s/%s", dir_arg, entries[i]->d_name);
            struct stat statbuf;
            if (stat(path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode) && strcmp(entries[i]->d_name, ".") != 0 && strcmp(entries[i]->d_name, "..") != 0) {
                printf("\n%s:\n", path);
                print_args(path, "NULL", flag_all, flag_long, flag_file, flag_reverse, flag_size, flag_recursive);
            }
        }
    }
}
