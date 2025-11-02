#!/usr/bin/python3
import os
import subprocess
import networkx as nx
import matplotlib.pyplot as plt

#Función para limpiar pantalla
def clear():
    os.system('clear')


#Obtener procesos del sistema
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


#Construir el grafo Padre -> Hijo
def construir_grafo(procesos):
    G = nx.DiGraph()
    for p in procesos:
        G.add_node(p["pid"], label=p["cmd"], user=p["user"])
        if p["ppid"] != "0" and p["ppid"] != p["pid"]:
            G.add_edge(p["ppid"], p["pid"])
    return G


#Función para resaltar nodos según tipo
def mostrar_grafo_resaltado(G, pid, modo):
    """
    modo puede ser:
    1 - solo el proceso
    2 - ascendientes
    3 - descendientes
    4 - ambos
    """
    plt.figure(figsize=(12, 8))
    pos = nx.spring_layout(G, k=0.4, iterations=50)

    # Colores base
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


#Función principal para ejecutar la actividad
def ejecutar_actividad2():
    clear()
    print("==============================")
    print("     ACTIVIDAD 2 - GRAFOS     ")
    print("==============================\n")
    print("1. Resaltar un proceso (PID)")
    print("2. Resaltar ascendientes")
    print("3. Resaltar descendientes")
    print("4. Resaltar ambos")
    print("0. Salir")

    opcion = int(input("\nElige una opción: "))

    if opcion == 0:
        print("Saliendo de la actividad 2...")
        return

    pid = input("Introduce el PID del proceso: ")

    procesos = obtener_procesos()
    G = construir_grafo(procesos)

    if pid not in G.nodes():
        print(f"No existe el proceso con PID {pid}.")
        return

    mostrar_grafo_resaltado(G, pid, opcion)



ejecutar_actividad2()