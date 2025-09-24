#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

int main(){
    pid_t ret = fork();

    if(ret == 0){
        std::cout << "Hijo:" << ret << std::endl;
        std::cout << "Hijo hijo:" << getpid();
    }
    else{
        std::cout << "Hijo padre:" << ret << std::endl;
        std::cout << "Padre padre:" << getpid() << std::endl;
    }
}