#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
int main( int argc, const char* argv[] )
{
    FILE* fp = fopen("/tmp/tested_file.txt", "w+");
    fprintf(fp, "This is testing for fprintf...\n");
    fclose(fp);
    
    FILE* fp2 = fopen("/tmp/tested_file2.txt", "w+");
    fprintf(fp2, "This is testing file2 for fprintf...\n");
    fclose(fp2);

    FILE* fp3 = fopen("/tmp/tested_file3.txt", "w+");
    fprintf(fp3, "This is testing file3 for fprintf...\n");
    fclose(fp3);

    FILE* f = fopen("./tested_file_test.txt", "w+");
    fprintf(f, "This is testing_file_test for fprintf...\n");
    fclose(f);

    int res = mkdir("/tmp/testing_dir", S_IRWXU);
    printf("The res of the mkdir 1 is %d\n", res);
    
    res = mkdir("./testing_dir", S_IRWXU);
    printf("The res of the mkdir 2 is %d\n", res);
    
    res = rmdir("/tmp/testing_dir");
    printf("The res of the rmdir is %d\n", res);
    
    res = rmdir("./testing_dir");
    printf("The res of the second rmdir is %d\n", res);

    res = chmod("/tmp/tested_file.txt", S_IRWXU);
    printf("The res of the chmod is %d\n", res);

    res = chown("/tmp/tested_file.txt", 500, 500);
    printf("The res of the chmod is %d\n", res);

    res = link("/tmp/tested_file.txt", "./tested_file.txt");
    printf("The res of the link is %d\n", res);

    res = symlink("/tmp/tested_file2.txt", "./tested_file2.txt");
    printf("The res of the symlink is %d\n", res);

    res = rename("/tmp/tested_file3.txt", "/tmp/tested_file4.txt");
    printf("The res of the rename is %d\n", res);

    res = unlink("/tmp/tested_file.txt");
    printf("The res of the unlink is %d\n", res);
    res = unlink("./tested_file.txt");
    printf("The res of the unlink2 is %d\n", res);
    res = unlink("./tested_file2.txt");
    printf("The res of the unlink3 is %d\n", res);
    res = unlink("/tmp/tested_file2.txt");
    printf("The res of the unlink 4 is %d\n", res);
    res = unlink("./tested_file_test.txt");
    printf("The res of the unlink5 is %d\n", res);
    res = unlink("/tmp/tested_file4.txt");
    printf("The res of the unlink6 is %d\n", res);
 
}

