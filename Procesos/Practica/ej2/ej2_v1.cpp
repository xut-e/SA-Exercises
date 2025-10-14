#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <sys/stat.h>

#define HIJOS 3
#define EXTREMOS 2
#define CAMPOS 2
#define LOG "registro.txt"
#define LOGFILTRADO "coincidencias.txt"

struct campos{
    char caracteres[256];
    int lineas;
};

int main(){

    int AB[EXTREMOS], BC[EXTREMOS], DC[EXTREMOS];
    campos func_extra;

    pipe(AB);
    pipe(BC);
    pipe(DC);

    for (int i = 0; i < HIJOS; i++)
    {
        if (fork() == 0)
        {
            if (i == 0)
            {
                std::string texto_escrito;

                close(AB[0]);

                std::cout << "Introduce el texto a capturar (escribe END para finalizar):" << std::endl;

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
                std::vector<char> texto_leido(1024);
                ssize_t bytes_leidos; //signed size type, si devuelve un numero positivo es que hay bytes, si devuelve 0 es EOF y si devuelve -1 es error, por eso debe ser ssize_t y no size_t
                
                int lineas = 0;

                close(AB[1]);
                close(BC[0]);

                FILE *archivo = fopen(LOG, "w");

                if (!archivo)
                {
                    std::cout << "Error al abrir el archivo " << LOG << std::endl;
                    exit(1);
                }

                while ((bytes_leidos = read(AB[0], texto_leido.data(), texto_leido.size())) > 0) //data() es un método que devuelve un puntero de memoria (lo que necesitan read() y write()). Esta condición lee bytes hasta que read devuelve 0 (EOF)
                {
                    fwrite(texto_leido.data(), 1, bytes_leidos, archivo); //puntero a la informacion, tamaño de cada elemento, cuantos elementos, puntero al archivo abierto con fopen
                    fflush(archivo); //usamos fflush para forzar la escritura en vez de que se quede guardado en RAM

                    write(BC[1], texto_leido.data(), bytes_leidos); //Era mucho más fácil controlar cuándo debía C empezar a leer mediante pipes que mediante archivos.
                    lineas++;
                }
                
                close(AB[0]);
                close(BC[1]);
                fclose(archivo);

                campos *punteroAFuncExtra = &func_extra;

                if (fork() == 0)
                {
                    std::cout << "Introduce los caracter/es por el/los que quieras filtrar: ";
                    std::cin >> func_extra.caracteres;

                    std::cout << "Introduce la cantidad de lineas que quieras mostrar: ";

                    do
                    {
                        std::cin >> func_extra.lineas;

                        if(func_extra.lineas < 1 || func_extra.lineas > 20){
                            std::cout << "Introduce un numero entre 1 y 20!" << std::endl;
                        }
                        if(func_extra.lineas > lineas){
                            std::cout << "Introduce un número menor o igual que " << lineas << "!" << std::endl;
                        }
                        
                    } while (func_extra.lineas < 1 || func_extra.lineas > 20 || func_extra.lineas > lineas);

                    close(DC[0]);

                    write(DC[1], punteroAFuncExtra, sizeof(campos));

                    close(DC[1]);
                    
                    exit(0);
                }
                
                wait(NULL);

                exit(0);
            }
            else if (i == 2)
            {
                close(AB[0]);
                close(AB[1]);
                close(BC[1]);

                campos inputsLeidos;
                campos *punteroAInputsLeidos = &inputsLeidos;

                close(DC[1]);

                read(DC[0], punteroAInputsLeidos, sizeof(campos));

                close(DC[0]);

                std::string caracteres = inputsLeidos.caracteres;
                int lineas;

                FILE *escritura = fopen(LOGFILTRADO, "w");

                if (!escritura)
                {
                    std::cout << "Error al abrir el archivo " << LOGFILTRADO << std::endl;
                    exit(1);
                }

                FILE *lectura = fdopen(BC[0], "r");

                if (!lectura)
                {
                    std::cout << "Error al abrir el archivo " << LOG << std::endl;
                    exit(1);
                }
                

                std::vector<char> archivo_leido(1024);
                int lineas_restantes = 0;

                while (fgets(archivo_leido.data(), archivo_leido.size(), lectura))
                {
                    if (lineas_restantes > 0)
                    {
                        fputs(archivo_leido.data(), escritura);
                        lineas_restantes--;
                        continue;
                    }
                    
                    if (strstr(archivo_leido.data(), caracteres.c_str()))
                    {
                        fputs(archivo_leido.data(), escritura);
                        lineas_restantes = inputsLeidos.lineas - 1;
                    }
                }

                fclose(lectura);
                fclose(escritura);
                
                exit(0);
            }            
        }
    }

    close(AB[0]);
    close(AB[1]);
    close(BC[0]);
    close(BC[1]);
    close(DC[0]);
    close(DC[1]);


    for (int i = 0; i < HIJOS; i++)
    {
        wait(NULL);
    }
    
    return 0;
}