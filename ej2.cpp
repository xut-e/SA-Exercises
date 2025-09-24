#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>

int main(){
    int rw[2];
    pipe(rw);
    pid_t ret = fork();
    
    if(ret != 0){

    close(rw[0]);
    write(rw[1], "Hola que tal?", 13);
    }
    else{
        char buffer[20];

        close(rw[1]);
        read(rw[0], buffer, sizeof(buffer));
        std::cout << buffer;
        exit(0);
    }
    return 0;
}