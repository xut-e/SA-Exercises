#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

#define LENGTH 64

// DEBUG FLAGS
#define DEBUG_DIVIDIR 1   // 1 para activar prints en dividir, 0 para desactivar
#define DEBUG_CIFRAR 1    // 1 para activar prints en cifrar, 0 para desactivar
#define DEBUG_FORK 1      // 1 para prints de fork y pipes, 0 para desactivar

struct PIPE{
    int rw[2];
};

struct MITAD{
    char half[LENGTH/2];
};

void cifrar(MITAD& mitades){
    for(int i = 0; i < LENGTH/2; i++){
        mitades.half[i] = mitades.half[i] + 1; 
#if DEBUG_CIFRAR
        std::cout << "Cifrando índice " << i << ": " << mitades.half[i] << std::endl;
#endif
    }
}

void dividir(char* pw, std::vector<MITAD>& mitades){
    int len = 0, mitad = 0;

    while(pw[len] != '\0'){
        len++;
    }

    if(len % 2 == 0){
        mitad = len / 2;
    }
    else{
        mitad = (len + 1) / 2;
    }

#if DEBUG_DIVIDIR
    std::cout << "Dividiendo contraseña de longitud " << len << " en mitades de " << mitad << std::endl;
#endif

    for(int i = 0; i < LENGTH; i++){
        if(i < mitad){
            mitades[0].half[i] = pw[i];
#if DEBUG_DIVIDIR
            std::cout << "Mitad 0[" << i << "] = " << mitades[0].half[i] << std::endl;
#endif
        }
        else{
            mitades[1].half[i - mitad] = pw[i];
#if DEBUG_DIVIDIR
            std::cout << "Mitad 1[" << i - mitad << "] = " << mitades[1].half[i - mitad] << std::endl;
#endif
        }
    }
}

int main(){
    std::vector<PIPE> pipes(2);
    std::vector<MITAD> mitades(2);

    for(int i = 0; i < 2; i++){
        pipe(pipes[i].rw);
#if DEBUG_FORK
        std::cout << "Pipe " << i << " creado: [" << pipes[i].rw[0] << ", " << pipes[i].rw[1] << "]" << std::endl;
#endif
    }

    char password[LENGTH] = {0}, crypted_pw[LENGTH] = {0}, crypted_m1[LENGTH/2] = {0}, crypted_m2[LENGTH/2] = {0}, buffer[LENGTH] = {0};

    int len = 0;
    while(password[len] != '\0') len++;
    int mitad = (len + 1) / 2;

    std::cout << "Introduce una contraseña: ";
    std::cin.getline(password, sizeof(password));

    for(int i = 0; i < 2; i++){
        pid_t ret = fork();
#if DEBUG_FORK
        std::cout << "Fork " << i << " creado, PID: " << ret << std::endl;
#endif

        if(ret == 0){
            close(pipes[i].rw[1]);
            read(pipes[i].rw[0], password, sizeof(password));
#if DEBUG_FORK
            std::cout << "Proceso hijo " << i << " leyó la contraseña: " << password << std::endl;
#endif
            dividir(password, mitades);
            cifrar(mitades[i]);

            close(pipes[0].rw[0]);
            write(pipes[0].rw[1], crypted_m1, mitad + 1);
            close(pipes[1].rw[0]);
            write(pipes[1].rw[1], crypted_m2, mitad + 1);
        }
    }        

    close(pipes[0].rw[1]);
    write(pipes[0].rw[1], password, sizeof(password));

    close(pipes[1].rw[1]);
    write(pipes[1].rw[1], password, sizeof(password));

    for(int i = 0; i < sizeof(password); i++){
        if(i < mitad){
            crypted_pw[i] = crypted_m1[i];
        }
        else{
            crypted_pw[i] = crypted_m2[i];
        }
    }

    std::cout << "La contraseña cifrada es: ";
    
    for(int i = 0; i < LENGTH; i++){
        std::cout << crypted_pw[i];
    }
    std::cout << std::endl;

    return 0;
}
