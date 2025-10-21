//Incluimos las librerías necesarias
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <sys/stat.h>

//Definimos variables globales que nos pyuede interesar cambiar más adelante
#define HIJOS 3
#define EXTREMOS 2
#define CAMPOS 2
#define LOG "registro.txt"
#define LOGFILTRADO "coincidencias.txt"

//Definimos la estructura campos para la funcionalidad adicional
struct campos{
    char caracteres[256];//Usamos char[] porque es más fiable para hacer read() y write() que std::string
    int lineas;
};

//Función principal
int main(){

    //Definimos las que seran nuestras pipes
    int AB[EXTREMOS], BC[EXTREMOS], DC[EXTREMOS];
    
    //Definimos una variable de tipo campos
    campos func_extra;

    //Hacemos pipes de todas
    pipe(AB);
    pipe(BC);
    pipe(DC);

    //Comenzamos la creación de pipes. He optado por no factorizarlo, como sí hice con la anterior porque cada hijo hace algo muy diferente y la única manera de diferenciarlos que tengo es con la iteración del padre en la que se han creado.
    for (int i = 0; i < HIJOS; i++)
    {
        //Comenzamos a crear procesos
        if (fork() == 0)
        {
            //Proceso A. Aseguramos bien el manejo de los extremos de las pipes con close().
            if (i == 0)
            {
                std::string texto_escrito;

                close(AB[0]);

                std::cout << "Introduce el texto a capturar (escribe END para finalizar):" << std::endl;

                //Mientras que se vaya escribiendo se mantiene el proceso A capturando, incluso si se escribe vacío "".
                while (std::getline(std::cin, texto_escrito))
                {
                    //Si el texto escrito coincide con END finalizamos el bucle.
                    if (texto_escrito == "END")
                    {
                        break;
                    }
                    //Escribimos lo capturado, manteniendo los saltos de línea
                    write(AB[1], texto_escrito.c_str(), texto_escrito.size()); //c_str() es un método de string qe devuelve un puntero al string. Como write no es de C++ sino de POSIX, requiere de ese tipo de formato.
                    write(AB[1], "\n", 1);
                }

                close(AB[1]);
                exit(0);
            }
            //Proceso B
            else if (i == 1)
            {
                std::vector<char> texto_leido(1024);
                ssize_t bytes_leidos; //signed size type, si devuelve un numero positivo es que hay bytes, si devuelve 0 es EOF y si devuelve -1 es error, por eso debe ser ssize_t y no size_t
                
                int lineas = 0;

                //Manejamos extremos del pipe
                close(AB[1]);
                close(BC[0]);

                //Creamos un puntero al archivo abierto que nos permimtirá escribir en el con write(), esto es importante porque como hemos comentado antes al ser un método de POSIX necesitamos que sea un puntero.
                FILE *archivo = fopen(LOG, "w");

                //Manejo de errores
                if (!archivo)
                {
                    std::cout << "Error al abrir el archivo " << LOG << std::endl;
                    exit(1);
                }

                //Mientras queden datos por leer mantiene el bucle
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

                //Creamos un puntero de tipo campos que apunte a una variable del mismo tipo. Esto nos permitira usar la "variable" con los métodos de POSIX.
                campos *punteroAFuncExtra = &func_extra;

                //Creamos un nuevo proceso (D). He optado por hacerlo de esta forma y nó crear otra vuelta al for principal porque de esta forma es más fácil de controlar ya que sólo debe ejecutarse cuando el proceso B haya terminado pero antes de mandar a C lo capturado..
                if (fork() == 0)
                {
                    //Input del usuario.
                    std::cout << "Introduce los caracter/es por el/los que quieras filtrar: ";
                    std::cin >> func_extra.caracteres;

                    std::cout << "Introduce la cantidad de lineas que quieras mostrar: ";

                    //Manejo de errores.
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

                    //Gestión de los extremos de las pipes.
                    close(DC[0]);

                    write(DC[1], punteroAFuncExtra, sizeof(campos));

                    close(DC[1]);
                    
                    exit(0);
                }
                
                //Hace que el proceso B espere al proceso D antes de terminr de mandar los datos al proceso C.
                wait(NULL);

                exit(0);
            }
            //Proceso C.
            else if (i == 2)
            {
                //Manejo de extremos de pipes.
                close(AB[0]);
                close(AB[1]);
                close(BC[1]);

                //Creamos, como en el proceso B la variable y el puntero a esa variable de tipo campos.
                campos inputsLeidos;
                campos *punteroAInputsLeidos = &inputsLeidos;

                close(DC[1]);

                read(DC[0], punteroAInputsLeidos, sizeof(campos));

                close(DC[0]);

                //Guardamos los caracteres en una variable tipo string ya que no podemos usar el objeto string dentro del método strstr().
                std::string caracteres = inputsLeidos.caracteres;
                int lineas;

                //Hacemos lo mismo que en el proceso B pero para escribir esta vez.
                FILE *escritura = fopen(LOGFILTRADO, "w");

                //Manejo de errores.
                if (!escritura)
                {
                    std::cout << "Error al abrir el archivo " << LOGFILTRADO << std::endl;
                    exit(1);
                }

                //Y aquí para leer.
                FILE *lectura = fdopen(BC[0], "r");

                //Más manejo de errores.
                if (!lectura)
                {
                    std::cout << "Error al abrir el archivo " << LOG << std::endl;
                    exit(1);
                }
                
                //Creamos un vector con los bytes leídos, lo que hará más sencillo el manejo de los datos para escribirlos después en el archivo.
                std::vector<char> archivo_leido(1024);
                int lineas_restantes = 0;

                //Mientras haya bits por leer/escribir del archivo mantenemos el bucle.
                while (fgets(archivo_leido.data(), archivo_leido.size(), lectura))
                {
                    //Si quedan lineas por filtrar las metemos en el archivo (nueva funcionalidad).
                    if (lineas_restantes > 0)
                    {
                        fputs(archivo_leido.data(), escritura);
                        lineas_restantes--;
                        continue;
                    }
                    
                    //Aquí cambié de strchr() para caracteres (filtrar por '@') a strstr() para filtrar por strings.
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

    //Cerramos los extremos abiertos para evitar errores
    close(AB[0]);
    close(AB[1]);
    close(BC[0]);
    close(BC[1]);
    close(DC[0]);
    close(DC[1]);

    //Esperamos a que todos los procesos terminen antes de terminar con el proceso padre.
    for (int i = 0; i < HIJOS; i++)
    {
        wait(NULL);
    }
    
    return 0;
}