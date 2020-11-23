#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <semaphore.h>

const int BUFFER_SIZE = 51;

int create_tmp() {

    char tmp_name[] = "/tmp/tmpf.XXXXXX";
    int tmp_fd = mkstemp(tmp_name);
    if ( tmp_fd == -1) {
        printf("error\n");
        exit(1);
    }
    int size = BUFFER_SIZE + 1;
    char array[size];
    for ( int i = 0; i < size; ++i ) {
        array[i] = '\0';
    }
    write(tmp_fd, array, size);
    return tmp_fd;

}


int main(int argc, char * argv[]) {


    int fd = create_tmp();
    int* memory1 = (int*) mmap(NULL,10,PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (memory1 == NULL) {
        printf("error mapping\n");
        exit(1);
    }

    const char* out_sem_name = "/o_s";  
    sem_unlink(out_sem_name);
    sem_t* out = sem_open(out_sem_name, O_CREAT, 777, 0);   

    pid_t childPID = fork();

    if (childPID == 0) {             // client
        printf("client\n");

        FILE * read = fopen("input.txt", "r");

        int size;

        fscanf(read, "%d", &size);

        int array[size];

        for (int i = 0; i < size; i++) {
            fscanf(read, "%d", &array[i]);
        }

        memory1[0] = size;
        for (int i = 0; i < size; i++) {
            memory1[i + 1] = array[i];
        }

        fclose(read);

        sem_post(out);
        exit(0);
    } else if (childPID > 0) {
        sem_wait(out); 
        printf("server\n");   

        int size = memory1[0];


        int array[size];

        for (int i = 1; i <= size; i++) {
            array[i - 1] = memory1[i];
            array[i - 1] *= 10;
        }

        FILE * write = fopen("output.txt", "w");

        for (int i = 0; i < size; i++) {
            fprintf(write, "%d ", array[i]);
        }

        fclose(write);

    }
    else {
        fprintf(stderr, "fork() was not finished successfully");
        return 1;
    }


	return 0;
}