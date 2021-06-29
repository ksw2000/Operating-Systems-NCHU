#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<sys/time.h>
#include<linux/fs.h>
#include<errno.h>
#include<time.h>
#include<string.h>

#define FILESIZE (512 * 1000 * 1000)    // 512 MB
#define ALIGNED (4 * 1000)              // 4 KB
#define DEFAULTDEVFILE "/dev/sda1"
#define TEMPFILE "temp.txt"
#define STARS "***********************************************************************"
#define elapsed(start, end) (1000000 * ((end)->tv_sec - (start)->tv_sec) +\
        ((end)->tv_usec - (start)->tv_usec))
typedef enum{false, true} bool;

int temp_fd;

void open_temp(const char* temp, bool read){
    temp_fd = open(temp, read ? (O_RDONLY | O_NONBLOCK) : (O_WRONLY | O_CREAT | O_TRUNC), 0666);
    if(temp_fd == -1){
        fprintf(stderr, "open(%s) error: %d", temp, errno);
        exit(EXIT_FAILURE);
    }
}

void create_temp(const char* temp){
    int i;
    open_temp(temp, false);
    char* buffer = calloc(1, ALIGNED);
    for(i=0; i < FILESIZE / ALIGNED; i++){
        write(temp_fd, buffer, ALIGNED);
    }
    free(buffer);
    fsync(temp_fd);
    close(temp_fd);
}

unsigned long int read_temp(const char* temp, bool rand){
    struct timeval start, end;
    char* buffer = malloc(ALIGNED);
    int i;
    open_temp(temp, true);
    gettimeofday(&start, NULL);
    for(i=0; i < FILESIZE / ALIGNED; i++){
        if(rand) lseek(temp_fd, (random() % (FILESIZE / ALIGNED)) * ALIGNED, SEEK_SET);
        read(temp_fd, buffer, ALIGNED);
    }
    gettimeofday(&end, NULL);
    free(buffer);
    close(temp_fd);
    return elapsed(&start, &end);
}

void set_read_ahead(const char* disk, int ra){
    int dev_fd = open(disk, O_RDWR | O_NONBLOCK);
    if(dev_fd == -1){
        if(errno == 13){
            fprintf(stderr, "open(%s) permission dinied.\n", disk);
        }else{
            fprintf(stderr, "open(%s) error: %d\n\ndf -h\n\n", disk, errno);
            system("df -h");
        }
        exit(EXIT_FAILURE);
    }
    if(ioctl(dev_fd, BLKRASET, ra)){
        fprintf(stderr, "ioctl error: %d\n\ndf -h\n\n", errno);
        system("df -h");
        exit(EXIT_FAILURE);
    }
    close(dev_fd);
}

void clear_cache(){
    system("sudo sh -c \"echo 3 > /proc/sys/vm/drop_caches\"");
}




int main(int argc, char** args){
    // STEP0: set random seed
    srand(time(NULL));

    // STEP1: set dev path and print some information for user
    char* dev = (argc > 1)? args[1] : DEFAULTDEVFILE;

    printf("Device file: %s\n", dev);
    if(argc == 1){
        printf("  - You can change this by passing device file path at first arguments.\n    ex: `%s /dev/sdb1`\n", args[0]);
        printf("  - If you do not know what is your device file path, check by `df -h`\n");
    }

    char cwd[1024];
    getcwd(cwd, 1024);
    printf("\n"STARS"\n Ensure that the path `%s` is in the disk `%s`\n"STARS"\n\n", cwd, dev);

    // STEP2: create a 500MB file
    create_temp(TEMPFILE);
    printf("created "TEMPFILE"\n\n");

    // STEP3: set read ahead size iteratively
    int i;
    int sizeList[] = {0, 16, 64};
    char output[4096] = "";
    for(i=0; i<sizeof(sizeList)/sizeof(sizeList[0]); i++){
        // set read ahead size
        set_read_ahead(dev, sizeList[i]);

        // clean cache
        clear_cache();

        // sequential read
        unsigned long int diff_seq = read_temp(TEMPFILE, false);

        // clean cache
        clear_cache();

        // random read
        unsigned long int diff_rand = read_temp(TEMPFILE, true);

        // output result
        char* res = malloc(sizeof(char) * 1024);
        sprintf(res, "set read ahead - %2d\n  - sequential read: %10.3f ms\n  - randomly read:   %10.3f ms\n", sizeList[i], diff_seq/1000.0, diff_rand/1000.0);
        printf("%s", res);
        strcat(output, res);
        free(res);
    }

    // create a file for recording result
    FILE* o = fopen("output.txt", "w+");
    if(!o){
        fprintf(stderr, "open(output.txt) error: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    fprintf(o, "%s", output);
    fclose(o);

    // reset read ahead
    set_read_ahead(dev, 256);

    return 0;
}
