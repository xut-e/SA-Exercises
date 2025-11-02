#!/usr/bin/python3
import os
import subprocess
import networkx as nx
import matplotlib.pyplot as plt

# Funciones comunes

def clear():
    os.system('clear')

# === ACTIVIDAD 1 ===

def obtener_procesos():
    comando = ["ps", "-eo", "pid,ppid,user,stat,%cpu,%mem,etime,cmd", "--no-headers"]
    resultado = subprocess.run(comando, capture_output=True, text=True)

    procesos = []
    for linea in resultado.stdout.strip().split("\n"):
        partes = linea.split(None, 7)
        if len(partes) == 8:
            pid, ppid, user, stat, cpu, mem, etime, cmd = partes
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


def construir_grafo(procesos):
    G = nx.DiGraph()

    for p in procesos:
        G.add_node(p["pid"], label=p["cmd"], user=p["user"])
        if p["ppid"] != "0" and p["ppid"] != p["pid"]:
            G.add_edge(p["ppid"], p["pid"])

    return G


def mostrar_grafo(G):
    plt.figure(figsize=(12, 8))
    pos = nx.spring_layout(G, k=0.4, iterations=50)

    nx.draw(G, pos, with_labels=True, node_color='lightblue',
            node_size=500, font_size=8, arrows=True)

    plt.title("Grafo de procesos del sistema")
    plt.show()


def mostrar_informacion_pid(pid):
    procesos = obtener_procesos()
    encontrado = False

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

    if not encontrado:
        print(f"No se encontró ningún proceso con PID {pid}.")


def actividad1():
    OPCIONES2 = ["Volver al menú", "Ver todos los procesos", "Ver información de un proceso específico"]
    tamaño2 = len(OPCIONES2)

    while True:
        clear()
        print("=========================\n   ACTIVIDAD 1\n=========================")
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
            G = construir_grafo(procesos)
            mostrar_grafo(G)
        elif eleccion2 == 2:
            pid = input("Introduce el PID del proceso: ")
            mostrar_informacion_pid(pid)
        else:
            print("Opción inválida.")
            os.system('sleep 1')

# === ACTIVIDAD 2 ===

def mostrar_grafo_resaltado(G, pid, modo):
    plt.figure(figsize=(12, 8))
    pos = nx.spring_layout(G, k=0.4, iterations=50)

    node_colors = []
    for n in G.nodes():
        if n == pid:
            node_colors.append('red')  # PID central
        elif modo in [2, 4] and nx.has_path(G, n, pid):
            node_colors.append('orange')  # Ascendientes
        elif modo in [3, 4] and nx.has_path(G, pid, n):
            node_colors.append('green')  # Descendientes
        else:
            node_colors.append('lightblue')

    nx.draw(G, pos, with_labels=True, node_color=node_colors,
            node_size=500, font_size=8, arrows=True)

    if modo == 1:
        titulo = f"Proceso resaltado PID {pid}"
    elif modo == 2:
        titulo = f"Ascendientes de PID {pid}"
    elif modo == 3:
        titulo = f"Descendientes de PID {pid}"
    else:
        titulo = f"Ascendientes y descendientes de PID {pid}"

    plt.title(titulo)
    plt.show()


def actividad2():
    while True:
        clear()
        print("==============================")
        print("     ACTIVIDAD 2 - GRAFOS     ")
        print("==============================\n")
        print("1. Resaltar un proceso (PID)")
        print("2. Resaltar ascendientes")
        print("3. Resaltar descendientes")
        print("4. Resaltar ambos")
        print("0. Volver al menú principal")

        try:
            opcion = int(input("\nElige una opción: "))
        except ValueError:
            print("Opción inválida.")
            return

        if opcion == 0:
            return

        pid = input("Introduce el PID del proceso: ")

        procesos = obtener_procesos()
        G = construir_grafo(procesos)

        if pid not in G.nodes():
            print(f"No existe el proceso con PID {pid}.")
            os.system('sleep 2')
            return

        mostrar_grafo_resaltado(G, pid, opcion)


# === MENÚ PRINCIPAL ===

def main():
    DELIMITADOR = "=========================\n          MENU\n========================="
    OPCIONES = ["Salir", "Ejecutar actividad 1", "Ejecutar actividad 2"]
    tamaño = len(OPCIONES)

    while True:
        clear()
        print(DELIMITADOR)
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

# EJECUCIÓN

main()
