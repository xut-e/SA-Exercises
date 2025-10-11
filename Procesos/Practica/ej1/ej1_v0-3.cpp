#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <cstring>

#define LENGTH 64

struct PIPE {
    int fd[2];
};

void cifrar(char* buffer, int length) {
    for (int i = 0; i < length; i++) {
        buffer[i] += 3;
    }
}

int main() {
    PIPE toChild[2], toParent[2];

    // Crear pipes (uno padre->hijo y otro hijo->padre para cada hijo)
    for (int i = 0; i < 2; i++) {
        pipe(toChild[i].fd);
        pipe(toParent[i].fd);
    }

    char password[LENGTH];
    std::cout << "Introduce tu contraseña: ";
    std::cin.getline(password, sizeof(password));

    int len = strlen(password);
    int half = len / 2;
    int extra = len % 2;

    for (int i = 0; i < 2; i++) {
        pid_t pid = fork();

        if (pid == 0) { // hijo
            close(toChild[i].fd[1]); // cerrar escritura
            close(toParent[i].fd[0]); // cerrar lectura

            char buf[LENGTH / 2 + 1];
            int n = read(toChild[i].fd[0], buf, sizeof(buf));
            buf[n] = '\0';

            cifrar(buf, n);
            write(toParent[i].fd[1], buf, n);

            exit(0);
        }
    }

    // Padre: enviar mitades a cada hijo
    close(toChild[0].fd[0]);
    close(toChild[1].fd[0]);
    close(toParent[0].fd[1]);
    close(toParent[1].fd[1]);

    write(toChild[0].fd[1], password, half + extra);               // primera mitad
    write(toChild[1].fd[1], password + half + extra, half);         // segunda mitad

    close(toChild[0].fd[1]);
    close(toChild[1].fd[1]);

    // Esperar a los hijos
    wait(NULL);
    wait(NULL);

    // Leer resultados
    char crypted[LENGTH];
    int n1 = read(toParent[0].fd[0], crypted, half + extra);
    int n2 = read(toParent[1].fd[0], crypted + n1, half);

    int total = n1 + n2;

    std::cout << "Tu contraseña encriptada es: ";
    for (int i = 0; i < total; i++) {
        std::cout << crypted[i];
    }
    std::cout << "\n";

    return 0;
}
