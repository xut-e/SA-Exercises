#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <cstdlib>

#define LENGTH 64

void cifrar(char* buffer, int length) {
    for (int i = 0; i < length; i++) {
        buffer[i] += 3;
    }
}

int main() {
    int toChild[2];   // Padre -> Hijos
    int toParent[2];  // Hijos -> Padre

    char password[LENGTH];
    do
    {
        std::cout << "Introduce tu contraseña: ";
        std::cin.getline(password, sizeof(password));
    } while (password > LENGTH);

    int len = strlen(password);
    int half = len / 2;
    int extra = len % 2;

    // ---------- HIJO 1 ----------
    pid_t pid1 = fork();

    if (pid1 == 0) {
        close(toChild[1]);  // no escribe
        close(toParent[0]); // no lee

        char buffer[LENGTH];
        int n = read(toChild[0], buffer, LENGTH);
        close(toChild[0]);

        cifrar(buffer, half + extra);
        write(toParent[1], buffer, half + extra);
        close(toParent[1]);
        exit(0);
    }

    // ---------- PADRE: fase 1 ----------
    close(toChild[0]);  // no lee
    close(toParent[1]); // no escribe

    // Enviar 1ª mitad
    write(toChild[1], password, half + extra);
    close(toChild[1]); // cerrar escritura para que hijo1 vea EOF

    char crypted[LENGTH];
    int n1 = read(toParent[0], crypted, half + extra);
    waitpid(pid1, NULL, 0);

    // ---------- Reabrir pipes para fase 2 ----------
    pipe(toChild);
    pipe(toParent);

    // ---------- HIJO 2 ----------
    pid_t pid2 = fork();
    
    if (pid2 == 0) {
        close(toChild[1]);
        close(toParent[0]);

        char buffer[LENGTH];
        int n = read(toChild[0], buffer, LENGTH);
        close(toChild[0]);

        cifrar(buffer, half);
        write(toParent[1], buffer, half);
        close(toParent[1]);
        exit(0);
    }

    // ---------- PADRE: fase 2 ----------
    close(toChild[0]);
    close(toParent[1]);

    // Enviar 2ª mitad
    write(toChild[1], password + half + extra, half);
    close(toChild[1]);

    int n2 = read(toParent[0], crypted + n1, half);
    waitpid(pid2, NULL, 0);

    int total = n1 + n2;

    std::cout << "Tu contraseña encriptada es: ";
    for (int i = 0; i < total; i++)
        std::cout << crypted[i];
    std::cout << "\n";

    return 0;
}
