#!/usr/bin/python3
import os
DELIMITADOR = "=========================\n          MENU\n========================="

OPCIONES = ["Salir", "Ejecutar actividad 1", "Ejecutar actividad 2"]

tamaño = len(OPCIONES)

while True:
    os.system('clear')
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

    elif eleccion == 2:
        print("Ejecucion actividad 2")

    os.system('sleep 1')
