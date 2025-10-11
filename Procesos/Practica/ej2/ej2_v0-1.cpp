#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string>

#define HIJOS 3
#define EXTREMOS 2

int main(){

    int AB[EXTREMOS], BC[EXTREMOS];

    pipe(AB);
    pipe(BC);

    for (int i = 0; i < HIJOS; i++)
    {
        if (fork() == 0)
        {
            if (i == 0)
            {
                std::string texto_escrito;
                close(AB[0]);
                while (std::getline(std::cin, texto_escrito))
                {
                    if (texto_escrito == "END")
                    {
                        break;
                    }
                    write(AB[1], texto_escrito.c_str(), texto_escrito.size()); //c_str() es un método de string qe devuelve un puntero al string. Como write no es de C++ sino de POSIX, requiere de ese tipo de formato.
                    write(AB[1], "\n", 1);
                }
                close(AB[1]);
                exit(0);
            }
            else if (i == 1)
            {
                std::string texto_leido;
                close(AB[1]);
                read(AB[0], texto_leido.c_str(), texto_leido.size());
                close(AB[0]);

                close(BC[0]);
                write();
            }
            else if (i == 2)
            {
                /* code */
            }
        }
    }
    
}