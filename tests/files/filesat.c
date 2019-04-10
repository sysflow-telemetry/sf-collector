#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
int main( int argc, const char* argv[] )
{

    DIR* dir = opendir("/tmp/");
    int dfd = dirfd(dir);

    printf("The dir fd is %d, Errno %d\n", dfd, errno);


    int f = openat(dfd, "tested_file.txt", O_WRONLY | O_CREAT);
    
    printf("The fd is %d, Errno %d\n", f, errno);

    if(f != -1) {
        char buff[100];
        strcpy(buff, "This is testing for fprintf...\n");
        write(f, buff, strlen(buff));
        close(f);
    }

   
    

    FILE* fp2 = fopen("/tmp/tested_file2.txt", "w+");
    fprintf(fp2, "This is testing file2 for fprintf...\n");
    fclose(fp2);

    FILE* fp3 = fopen("/tmp/tested_file3.txt", "w+");
    fprintf(fp3, "This is testing file3 for fprintf...\n");
    fclose(fp3);

    FILE* fp = fopen("./tested_file_test.txt", "w+");
    fprintf(fp, "This is testing_file_test for fprintf...\n");
    fclose(fp);

    int res = mkdirat(dfd, "/tmp/testing_dir", S_IRWXU);
    printf("The res of the mkdir 1 is %d\n", res);
    
    res = mkdirat(AT_FDCWD, "./testing_dir", S_IRWXU);
    printf("The res of the mkdir 2 is %d\n", res);
    
    res = rmdir("/tmp/testing_dir");
    printf("The res of the rmdir is %d\n", res);
    
    res = rmdir("./testing_dir");
    printf("The res of the second rmdir is %d\n", res);

    //res = chmod("/tmp/tested_file.txt", S_IRWXU);
    //printf("The res of the chmod is %d\n", res);

    //res = chown("/tmp/tested_file.txt", 500, 500);
    //printf("The res of the chmod is %d\n", res);

    res = linkat(dfd, "./tested_file.txt", AT_FDCWD,  "./tested_file.txt", 0);
    printf("The res of the link is %d\n", res);

    res = symlinkat("/tmp/tested_file2.txt", AT_FDCWD,  "./tested_file2.txt");
    printf("The res of the symlink is %d\n", res);

    res = renameat(dfd, "./tested_file3.txt", AT_FDCWD,  "/tmp/tested_file4.txt");
    printf("The res of the rename is %d\n", res);

    res = unlinkat(dfd, "./tested_file.txt", 0);
    printf("The res of the unlink is %d\n", res);
    res = unlinkat(AT_FDCWD, "./tested_file.txt", 0);
    printf("The res of the unlink2 is %d\n", res);
    res = unlinkat(AT_FDCWD, "./tested_file2.txt", 0);
    printf("The res of the unlink3 is %d\n", res);
    res = unlinkat(dfd, "/tmp/tested_file2.txt", 0);
    printf("The res of the unlink 4 is %d\n", res);
    res = unlinkat(AT_FDCWD, "./tested_file_test.txt", 0);
    printf("The res of the unlink5 is %d\n", res);
    res = unlinkat(dfd, "../tmp/tested_file4.txt", 0);
    printf("The res of the unlink6 is %d\n", res);
    closedir(dir);
}

