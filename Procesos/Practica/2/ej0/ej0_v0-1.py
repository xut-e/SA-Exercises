#!/usr/bin/python3
import os
DELIMITADOR = "=========================\n          MENU\n========================="

OPCIONES = ["Salir", "Ejecutar actividad 1", "Ejecutar actividad 2"]
OPCIONES2 = ["Volver al menu", "Ver todos los procesos", "Ver informacion de un proceso en específico"]

tamaño = len(OPCIONES)
tamaño2 = len(OPCIONES2)

def clear():
    os.system('clear')

clear()

while True:
    clear
    print(DELIMITADOR)
    
    eleccion = -1

    for opciones in range(tamaño):
        print(opciones,".", OPCIONES[opciones])
    while True:
        eleccion = int(input("Elige una opcion: "))

        if eleccion < 0 or eleccion > tamaño:
            print("Elige una opcion valida!")
        else:
            break

    if eleccion == 0:
        print("Adios!")
        break

    elif eleccion == 1:
        print("Ejecución actividad 1")
        clear()
        print(DELIMITADOR)
        
        eleccion2 = -1

        for opciones2 in range(tamaño2):
            print(opciones2,".", OPCIONES2[opciones2])
        while True:
            eleccion2 = int(input("Elige una opcion: "))

            if eleccion2 < 0 or eleccion2 > tamaño2:
                print("Elige una opcion valida!")
            else:
                break

        if eleccion2 == 0:
            print("Volviendo al menu...")
            os.system('sleep 1')
            clear()
            continue
        elif eleccion2 == 1:
            print("Mostrando todos los procesos...")
            os.system('sleep 1')
            clear()
        elif eleccion2 == 2:
            pid = input("Introduce el PID del proceso: ")
            print(f"Mostrando informacion del proceso {pid}...")
            os.system('sleep 1')
            clear()


    elif eleccion == 2:
        print("Ejecucion actividad 2")

    os.system('sleep 1')
