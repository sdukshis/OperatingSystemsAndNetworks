/* list all file names in directory */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

int main(int argc, char *argv[]) {
    const char *dirpath = "."; /* working dir by default */
    if (argc > 1) {
        dirpath = argv[1];
    }

    DIR *dirp = opendir(dirpath);
    if (dirp == NULL) {
        perror("opendir");
        return EXIT_FAILURE;
    }
    struct dirent *direntp = NULL;
    while ((direntp = readdir(dirp)) != NULL) {
        printf("%s\n", direntp->d_name);
    }
    closedir(dirp);
    return EXIT_SUCCESS;
}