/* prints file info */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

const int PERM_SIZE = 10;

const char *filetype(mode_t mode); /* return file type as string */
void fileperm(mode_t mode, char out[]); /* return file permissionas as string */

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filepath>\n", argv[0]);
        return EXIT_FAILURE;
    }
    struct stat sb;
    if (-1 == stat(argv[1], &sb)) {
        perror("stat");
        return EXIT_FAILURE;
    }
    printf("%s\n", argv[1]);
    printf("    inode:    %ju\n", (uintmax_t)sb.st_ino);
    printf("    type:     %s\n", filetype(sb.st_mode));
    printf("    uid:      %ju\n", (uintmax_t)sb.st_uid);
    printf("    gid:      %ju\n", (uintmax_t)sb.st_gid);
    printf("    size:     %jd\n", (intmax_t)sb.st_size);
    printf("    links:    %ju\n", (uintmax_t)sb.st_nlink);
    char perms[PERM_SIZE];
    fileperm(sb.st_mode, perms);
    printf("    perms:    %s\n", perms);
    printf("    modified: %s", ctime(&sb.st_mtime));

    return EXIT_SUCCESS;
}

const char *filetype(mode_t mode) {
    mode_t type = mode & S_IFMT;
    switch(type) {
        case S_IFREG:
            return "regular";
        case S_IFDIR:
            return "directory";
        case S_IFCHR:
            return "character device";
        case S_IFBLK:
            return "block device";
        case S_IFIFO:
            return "FIFO";
        case S_IFLNK:
            return "symbolic link";
        case S_IFSOCK:
            return "socket";
        default:
            return "unknown";
    }
}

void fileperm(mode_t mode, char out[]) {
    const char* zeroperms = "---------";
    strncpy(out, zeroperms, strlen(zeroperms));
    if (S_IRUSR & mode) {
        out[0] = 'r';
    }
    if (S_IWUSR & mode) {
        out[1] = 'w';
    }
    if (S_IXUSR & mode) {
        out[2] = 'x';
    }
    if (S_IRGRP & mode) {
        out[3] = 'r';
    }
    if (S_IWGRP & mode) {
        out[4] = 'w';
    }
    if (S_IXGRP & mode) {
        out[5] = 'x';
    }
    if (S_IROTH & mode) {
        out[6] = 'r';
    }
    if (S_IWOTH & mode) {
        out[7] = 'w';
    }
    if (S_IXOTH & mode) {
        out[8] = 'x';
    }
}