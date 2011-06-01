#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/prctl.h>

#define FLOAT_SIZE 4096

unsigned int sleep_time = 50000;

unsigned char *mem_buff;

void *busy_cpu(void *arg)
{
    prctl(PR_SET_NAME, (unsigned int)"busyCPU", 0, 0, 0);

    int i;

    double *farray = (double*)malloc(FLOAT_SIZE*sizeof(double));
    double total = 0.0f;

    if (farray == NULL) {
        perror("busyCPU malloc");
        return NULL;
    }

    for (i = 0; i < FLOAT_SIZE; ++i)
        farray[i] = (double)rand();

    while (1) {
        for (i = 0; i < FLOAT_SIZE; ++i)
            total *= farray[i];

        for (i = 0; i < FLOAT_SIZE; ++i)
            total -= farray[i];

        farray[rand()%FLOAT_SIZE] = total;

        usleep(sleep_time);
    }

    return NULL;
}

void *more_mem(unsigned int mem_size)
{
    prctl(PR_SET_NAME, (unsigned int)"moreMEM", 0, 0, 0);
    printf("moreMEM thread created, mem_size: %d\n", mem_size);

    int i;
    unsigned char *reverse_buff = (unsigned char*)malloc(mem_size);
    if (reverse_buff == NULL) {
        perror("moreMEM malloc:");
        return NULL;
    }

    while (1) {
        for (i = 0; i < mem_size; ++i)
            ++mem_buff[i];

        usleep(sleep_time);

        for (i = 0; i < mem_size; ++i)
            reverse_buff[mem_size-i] = mem_buff[i];

        usleep(sleep_time);
    }

    return NULL;
}

void *write_sd(unsigned int mem_size)
{
    prctl(PR_SET_NAME, (unsigned int)"writeSD", 0, 0, 0);

    int fd;

    printf("WriteSD thread created, mem_size: %d\n", mem_size);

    fd = open("/storage/data.tmp", O_WRONLY);
    if (fd == -1) {
        perror("open data.tmp");
        return NULL;
    }

    while (1) {
        write(fd, mem_buff, mem_size);
        sync();
        usleep(sleep_time);
    }

    close(fd);
    return NULL;
}

int main(int argc, char **argv)
{
    unsigned int mem_size;
    pthread_t tid1, tid2, tid3;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s mem_size sleep_usec\n", argv[0]);
        exit(1);
    }

    srand(time(0));

    mem_size = atoi(argv[1]);
    sleep_time = atoi(argv[2]);

    mem_buff = malloc(mem_size);
    if (mem_buff == NULL) {
        perror("malloc");
        exit(1);
    }

    pthread_create(&tid1, NULL, busy_cpu, (void*)mem_size);
    pthread_create(&tid2, NULL, more_mem, (void*)mem_size);
    pthread_create(&tid3, NULL, write_sd, (void*)mem_size);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

    return 0;
}
