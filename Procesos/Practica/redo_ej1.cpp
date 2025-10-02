#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <cstring>


#define LENGTH 64
#define PIPES 2
struct PIPE{
    int pipe[PIPES];
};

void cifrar(char* buffer, int length){

    for (int i = 0; i < length; i++)
    {
        buffer[i] += 3;
    }
    
}

int main(){
    std::vector<PIPE> rw(2);

    for (int i = 0; i < PIPES; i++)
    {
        pipe(rw[i].pipe);
    }

    char password[LENGTH], buffer_p1[LENGTH/2], buffer_p2[LENGTH/2], crypted[LENGTH];
    bool flag = false;
    
    do
    {
        std::cout << "Introduce tu contraseña: ";
        std::cin.getline(password, sizeof(password));
    } while (sizeof(password) > LENGTH);
    
    int len = strlen(password);

    int half_length;

    if(len % 2 == 0){
        half_length = len / 2;
    }
    else{
        half_length = len / 2 + 1;
        flag = true;
    }

    for (int i = 0; i < PIPES; i++)
    {
        pid_t ret = fork();

        if (ret == 0)
        {
            close(rw[1].pipe[1]);

            if (i == 0)
            {
                read(rw[0].pipe[0], buffer_p1, half_length);

                cifrar(buffer_p1, half_length);

                close(rw[1].pipe[0]);
                write(rw[1].pipe[1], buffer_p1, half_length);

                exit(0);
            }
            else
            {
                if(flag) half_length--;
                
                read(rw[0].pipe[0], buffer_p2, half_length);

                cifrar(buffer_p2, half_length);

                close(rw[1].pipe[0]);
                write(rw[1].pipe[1], buffer_p2, half_length);

                exit(0);
            }
            
        }
        
    }

    close(rw[0].pipe[0]);
    write(rw[0].pipe[1], password, len);

    close(rw[1].pipe[1]);

    wait(NULL);
    wait(NULL);
    
    if (!flag)
    {
        read(rw[1].pipe[0], crypted, half_length * 2);
    }
    else
    {
        read(rw[1].pipe[0], crypted, half_length * 2 - 1);
    }
    

    std::cout << "Tu contraseña encriptada es: ";

    for (int i = 0; i < len; i++)
    {
        std::cout << crypted[i];
    }
    
    std::cout << "\n";
    

    return 0;
}