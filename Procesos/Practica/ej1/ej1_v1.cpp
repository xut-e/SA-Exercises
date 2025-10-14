//Incluimos las librerías necesarias.
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string>

//Definimos variables que nos puede interesar cambiar más adelante.
#define PIPE_GROUPS 2
#define PROCESOS_HIJOS 2
#define PIPES_POR_GRUPO 2
#define DIVISION_PALABRAS 2

//Hacemos una estructura para cada comunicación (como cada comunicación será obtenida mediante dos pipes) ----> pipe_group[(A-->B),(B-->A)].
struct ReadWrite{
    int pipe_group[PIPES_POR_GRUPO][2];
};

//Función de cifrado, esta es un cesar +3, super sencilla, pero se puede complicar mucho más, encuentro que para este ejercicio no tiene ventajas. Le pasamos mitad como dirección, para que pueda editarla.
void cifrar(std::string& mitad){

    //Sumamos 3 espacios a cada caracter (técnicamente no es cesar+3 porque la z no vuelve a comenzar el abecedario, pero para el ejemplo es más que suficiente).
    for (size_t i = 0; i < mitad.size(); i++)
    {
        mitad[i] += 3;
    }
    
}

//Función principal
int main(){
    //Declaramos el vector que almacenará nuestros pipes de comunicación, dos por cada comunicación.
    std::vector<ReadWrite> pipes(PIPE_GROUPS);
    std::string mitad1, mitad2, password;
    std::vector<std::string> mitad_inicial(DIVISION_PALABRAS), mitad_crypt(DIVISION_PALABRAS), mitad_final(DIVISION_PALABRAS);
    int mitad = 0;

    for (size_t i = 0; i < PIPE_GROUPS; i++)
    {
        for (size_t j = 0; j < PIPES_POR_GRUPO; j++)
        {
            pipe(pipes[i].pipe_group[j]);
        }
    }
    
    std::cout << "Escribe tu contraseña : ";
    std::getline(std::cin, password);

    mitad = password.size() / 2;

    if (password.size() % 2 != 0)
    {
        mitad += 1;
    }
    
    mitad_inicial[0] = password.substr(0, mitad);
    mitad_inicial[1] = password.substr(mitad);

    for (size_t i = 0; i < DIVISION_PALABRAS; i++)
    {
        mitad_crypt[i].resize(mitad_inicial[i].size());
        mitad_final[i].resize(mitad_inicial[i].size());        
    }
    

    for (size_t i = 0; i < PROCESOS_HIJOS; i++)
    {
        if (fork() == 0)
        {
            close(pipes[i].pipe_group[0][1]);

            read(pipes[i].pipe_group[0][0], mitad_crypt[i].data(), mitad_crypt[i].size());

            close(pipes[i].pipe_group[0][0]);

            cifrar(mitad_crypt[i]);

            close(pipes[i].pipe_group[1][0]);

            write(pipes[i].pipe_group[1][1], mitad_crypt[i].data(), mitad_crypt[i].size());

            close(pipes[i].pipe_group[1][1]);
            
            exit(0);
        }
        
    }

    for (size_t i = 0; i < PIPE_GROUPS; i++)
    {
        close(pipes[i].pipe_group[0][0]);

        write(pipes[i].pipe_group[0][1], mitad_inicial[i].data(), mitad_inicial[i].size());

        close(pipes[i].pipe_group[0][1]);
    }
    

    wait(NULL);
    wait(NULL);

    std::cout << "La contraseña final es: ";

    for (size_t i = 0; i < PIPE_GROUPS; i++)
    {
        close(pipes[i].pipe_group[0][1]);

        read(pipes[i].pipe_group[1][0], mitad_final[i].data(), mitad_final[i].size());

        close(pipes[i].pipe_group[1][0]);

        close(pipes[i].pipe_group[1][1]);

        std::cout << mitad_final[i];
    }
    
    
    
}