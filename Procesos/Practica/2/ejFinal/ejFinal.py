#!/usr/bin/python3

#Importamos las librerías necesarias
import os
import subprocess
import networkx as nwx
import matplotlib.pyplot as plot

# Funciones comunes
def clear():
    os.system('clear')

#ACTIVIDAD 1

#Obtiene una lista de procesos del sistema usando ps y devuelve una lista de diccionarios cada uno con información de un proceso
def obtener_procesos():
    #Definimos el comando que se ejecutará por detras para presentar el gráfico
    comando = ["ps", "-eo", "pid,ppid,user,stat,%cpu,%mem,etime,cmd", "--no-headers"]
    #Ejecutamos dicho comando y capturamos el output
    resultado = subprocess.run(comando, capture_output=True, text=True)
    #Definimos una lista para meter los procesos
    procesos = []
    #Dividimos cada linea hasta en 8 partes
    for linea in resultado.stdout.strip().split("\n"):
        partes = linea.split(None, 7)

        if len(partes) == 8:
            pid, ppid, user, stat, cpu, mem, etime, cmd = partes
            #Añadimos el proceso a la lista
            procesos.append({
                "pid": pid,
                "ppid": ppid,
                "user": user,
                "stat": stat,
                "cpu": cpu,
                "mem": mem,
                "etime": etime,
                "cmd": cmd
            })

    return procesos

#Construye un grafo a partir de la lista de procesos. Cada nodo representa un proceso y las líneas las relaciones padre-hijo
def construir_grafo(procesos):
    grafico = nwx.DiGraph()

    for p in procesos:
        #Añadimos el nodo con atributos
        grafico.add_node(p["pid"], label=p["cmd"], user=p["user"])
        #Añadimos la relación padre hijo si el proceso tiene más de un PPID válido
        if p["ppid"] != "0" and p["ppid"] != p["pid"]:
            grafico.add_edge(p["ppid"], p["pid"])

    return grafico

#Mostramos el grafo en la pantalla usando matplotlib.
def mostrar_grafo(grafico):
    plot.figure(figsize=(12, 8))
    #Guardamos la posición del nodo en el plot
    pos = nwx.spring_layout(grafico, k=0.4, iterations=50)
    #Lo dibujamos indicando todas las condiciones
    nwx.draw(grafico, pos, with_labels=True, node_color='lightblue',
            node_size=500, font_size=8, arrows=True)

    plot.title("Grafo de procesos del sistema")
    #Lo mostramos
    plot.show()

#Muestra la información detallada de un proceso para un PID concreto
def mostrar_informacion_pid(pid):
    #Llamamos a obtener_procesos() para guardarnos la lista de los procesos
    procesos = obtener_procesos()
    #Encontrado inicializado en falso
    encontrado = False
    #Si coincide ek pid con algún proceso mostramos la info
    for p in procesos:
        if p["pid"] == str(pid):
            encontrado = True

            print("===============================")
            print(f"PID:   {p['pid']}")
            print(f"PPID:  {p['ppid']}")
            print(f"Usuario: {p['user']}")
            print(f"Estado:  {p['stat']}")
            print(f"%CPU:    {p['cpu']}")
            print(f"%MEM:    {p['mem']}")
            print(f"Tiempo:  {p['etime']}")
            print(f"Comando: {p['cmd']}")
            print("===============================")
            input("Presiona Enter para continuar...")

            break
    #Si no decimos que no se encontró el proceso con dicho PID
    if not encontrado:
        print(f"No se encontró ningún proceso con PID {pid}.")

#Menú para la actividad 1. Muestra las opciones y permite seleccionar una gestionando errores de input
def actividad1():
    OPCIONES2 = ["Volver al menú", "Ver todos los procesos", "Ver información de un proceso específico"]

    tamaño2 = len(OPCIONES2)

    while True:
        clear()

        print("==============================")
        print("          ACTIVIDAD 1         ")
        print("==============================")
        for i in range(tamaño2):
            print(i, ".", OPCIONES2[i])

        try:
            eleccion2 = int(input("Elige una opción: "))
        except ValueError:
            print("Introduce un número válido.")
            os.system('sleep 1')
            continue

        if eleccion2 == 0:
            return
        elif eleccion2 == 1:
            print("Mostrando todos los procesos...")
            procesos = obtener_procesos()
            grafico = construir_grafo(procesos)
            mostrar_grafo(grafico)
        elif eleccion2 == 2:
            pid = input("Introduce el PID del proceso: ")
            mostrar_informacion_pid(pid)
        else:
            print("Opción inválida.")
            os.system('sleep 1')

#ACTIVIDAD 2
#Muestra el gráfico resaltado con diferentes colores en función de la opción escogida y el PID dado
def mostrar_grafo_resaltado(grafico, pid, modo):
    plot.figure(figsize=(12, 8))

    pos = nwx.spring_layout(grafico, k=0.4, iterations=50)

    node_colors = []

    for n in grafico.nodes():
        if n == pid:
            node_colors.append('red')  # PID central
        elif modo in [2, 4] and nwx.has_path(grafico, n, pid):
            node_colors.append('orange')  # Ascendentes
        elif modo in [3, 4] and nwx.has_path(grafico, pid, n):
            node_colors.append('green')  # Descendientes
        else:
            node_colors.append('lightblue')

    nwx.draw(grafico, pos, with_labels=True, node_color=node_colors, node_size=500, font_size=8, arrows=True)

    if modo == 1:
        titulo = f"Proceso resaltado PID {pid}"
    elif modo == 2:
        titulo = f"Ascendientes de PID {pid}"
    elif modo == 3:
        titulo = f"Descendientes de PID {pid}"
    else:
        titulo = f"Ascendientes y descendientes de PID {pid}"

    plot.title(titulo)

    plot.show()

#Muestra el menú de la actividad 2
def actividad2():
    while True:
        clear()

        print("==============================")
        print("     ACTIVIDAD 2 - GRAFOS     ")
        print("==============================")
        print("0. Volver al menú principal")
        print("1. Resaltar un proceso (PID)")
        print("2. Resaltar ascendientes")
        print("3. Resaltar descendientes")
        print("4. Resaltar ambos")
        

        try:
            opcion = int(input("\nElige una opción: "))
        except ValueError:
            print("Opción inválida.")
            return

        if opcion == 0:
            return

        pid = input("Introduce el PID del proceso: ")

        procesos = obtener_procesos()
        grafico = construir_grafo(procesos)

        if pid not in grafico.nodes():
            print(f"No existe el proceso con PID {pid}.")
            os.system('sleep 2')
            return

        mostrar_grafo_resaltado(grafico, pid, opcion)


#MENÚ PRINCIPAL
#Muestra el menú principal y maneja el flujo principal
def main():
    OPCIONES = ["Salir", "Ejecutar actividad 1", "Ejecutar actividad 2"]

    tamaño = len(OPCIONES)

    while True:
        clear()

        print("==============================")
        print("        MENU PRINCIPAL        ")
        print("==============================")

        for i in range(tamaño):
            print(i, ".", OPCIONES[i])

        try:
            eleccion = int(input("Elige una opción: "))
        except ValueError:
            print("Introduce un número válido.")
            os.system('sleep 1')

            continue

        if eleccion == 0:
            print("¡Adiós!")

            break
        elif eleccion == 1:
            actividad1()
        elif eleccion == 2:
            actividad2()
        else:
            print("Opción inválida.")
            os.system('sleep 1')


#Ejecuta el programa al llamarlo desde consola
main()
