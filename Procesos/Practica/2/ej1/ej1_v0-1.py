#!/usr/bin/python3
import os
import subprocess
import networkx as nx
import matplotlib.pyplot as plt

#Función para limpiar la pantalla
def clear():
    os.system('clear')


#Función: obtener información de todos los procesos del sistema
def obtener_procesos():
    """
    Ejecuta el comando ps y devuelve una lista de diccionarios
    con la información de cada proceso activo.
    """
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


#Función: construir el grafo de procesos
def construir_grafo(procesos):
    """
    Construye un grafo dirigido Padre -> Hijo
    usando networkx.DiGraph()
    """
    G = nx.DiGraph()

    for p in procesos:
        G.add_node(p["pid"], label=p["cmd"], user=p["user"])
        if p["ppid"] != "0" and p["ppid"] != p["pid"]:
            G.add_edge(p["ppid"], p["pid"])

    return G


#Función: mostrar el grafo con matplotlib
def mostrar_grafo(G):
    """
    Dibuja el grafo de procesos con etiquetas de PID.
    """
    plt.figure(figsize=(12, 8))
    pos = nx.spring_layout(G, k=0.4, iterations=50)

    nx.draw(G, pos, with_labels=True, node_color='lightblue',
            node_size=500, font_size=8, arrows=True)

    plt.title("Grafo de procesos del sistema")
    plt.show()


#Función: mostrar información detallada de un proceso por PID
def mostrar_informacion_pid(pid):
    """
    Muestra la información de un proceso específico.
    """
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
            break

    if not encontrado:
        print(f"No se encontró ningún proceso con PID {pid}.")


#Funciones principales para usar desde el menú
def actividad1_ver_todos():
    """
    Muestra todos los procesos del sistema como grafo.
    """
    clear()
    print("Obteniendo información de procesos...")
    procesos = obtener_procesos()
    G = construir_grafo(procesos)
    mostrar_grafo(G)


def actividad1_ver_pid(pid):
    """
    Muestra información de un proceso en particular.
    """
    clear()
    mostrar_informacion_pid(pid)


#Test manual (solo si se ejecuta directamente)

clear()
print("=== TEST Actividad 1 ===")
print("1. Ver todos los procesos")
print("2. Ver información de un PID concreto")
print("0. Salir")

opcion = int(input("Selecciona una opción: "))

if opcion == 1:
    actividad1_ver_todos()
elif opcion == 2:
    pid = input("Introduce el PID: ")
    actividad1_ver_pid(pid)
else:
    print("Saliendo...")
