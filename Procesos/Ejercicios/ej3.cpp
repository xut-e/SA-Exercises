#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>

int main(){
    int rw1[2], rw2[2];

    pipe(rw1);
    pipe(rw2);

    pid_t ret = fork();

    char mensajePlano[64];
    
    if(ret != 0){
        std::cout << "Introduce un mensaje: ";
        std::cin.getline(mensajePlano, sizeof(mensajePlano));
        char cifrado[64];
        
        int len = 0;

        while(mensajePlano[len] != '\0'){
            len++;
        }

        close(rw1[0]);
        write(rw1[1], mensajePlano, len + 1);
        close(rw2[1]);
        read(rw2[0], cifrado, sizeof(cifrado));
        std::cout << "El mensaje cifrado es:" << cifrado << std::endl;

        wait(NULL);
    }
    else{
        char buffer[64];

        close(rw1[1]);
        read(rw1[0], buffer, sizeof(buffer));

        for(int i = 0; buffer[i] != '\0'; i++){
            buffer[i] += 1;
        }

        close(rw2[0]);
        write(rw2[1], buffer, sizeof(buffer));

        exit(0);
    }
    return 0;
}