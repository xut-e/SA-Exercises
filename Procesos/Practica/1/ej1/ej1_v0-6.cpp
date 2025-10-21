#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>

#define PIPES 2
#define LENGTH 64

struct PIPE{
    int extremos[2];
};

void cifrar(char* pass, int length){
    for (int i = 0; i < length; i++)
    {
        pass[i] = pass[i] + 1;
    }
}

int main(){

    char password[LENGTH] = {'\0'}, pass1[LENGTH/2], pass2[LENGTH/2], half1[LENGTH/2], half2[LENGTH/2];
    int longitud = 0;

    std::vector<PIPE> pipes(PIPES);
    std::vector<char*> pass(PIPES);

    for (int i = 0; i < PIPES; i++) {
        pipe(pipes[i].extremos);
    }

    std::cout << "Por favor introduzca su contraseña: ";
    std::cin.getline(password, LENGTH);

    // Dividimos la contraseña en dos mitades

    do
    {
        longitud++;
    } while (password[longitud] != '\0');
    
    if (longitud % 2 == 0)
    {
        for (int i = 0; i < longitud/2; i++)
        {
            half1[i] = password[i];
            half2[i] = password[i + longitud/2];
        }
    } else {
        for (int i = 0; i < longitud/2; i++)
        {
            half1[i] = password[i];
            half2[i] = password[i + longitud/2 + 1];
        }
        half2[longitud/2] = password[longitud - 1];
    }

    
    
    
    for (int i = 0; i < PIPES; i++)
    {
        pid_t ret = fork();
        
        if (ret == 0)
        {
            read(pipes[i].extremos[0], pass[i], sizeof(pass[i]));

            cifrar(pass[i], LENGTH/2);

            close(pipes[i].extremos[0]);

            for (int j = 0; j < sizeof(pass); j++)
            {
                half1[j] = pass[0][j];
                half2[j] = pass[1][j];
            }
            

            write(pipes[i].extremos[1], pass[i], sizeof(pass[i]));

            close(pipes[i].extremos[1]);

            exit(0);
        }
        
    }

    close(pipes[0].extremos[0]);
    close(pipes[1].extremos[0]);

    write(pipes[0].extremos[1], half1, sizeof(half1));
    write(pipes[1].extremos[1], half2, sizeof(half2));

    close(pipes[0].extremos[1]);
    close(pipes[1].extremos[1]);    

    wait(NULL);
    wait(NULL);
    
    std::cout << "Contraseña cifrada: " << half1 << half2 << std::endl;

    return 0;
}