#!/usr/bin/env python3
"""
Universidad del Valle de Guatemala
Departamento de Computación
Computación Paralela - CC3069

Consultoria Apex Compute

Autores:
   - Angie Vela, 23764
   - Cristian Tunchez, 231159

Script de benchmarking 

Compila las versiones secuencial y paralela, corre varias configuraciones
de numero de threads y de schedule, promedia varias corridas por
configuracion, y calcula speedup y eficiencia. Guarda los resultados en
un CSV y genera graficas de speedup y eficiencia vs. numero de threads.

Uso:
    python3 benchmark.py

Configuracion: editen las variables en la seccion CONFIG mas abajo segun
lo que necesiten probar (n, threads a probar, schedules, repeticiones).
"""

import subprocess
import csv
import statistics
import os
import sys

# Cada integrante debe ejecutar esta configuración completa en su propia máquina.
# Mantengan N, A y B iguales entre el baseline y todas las variantes paralelas.
# ------------------------- CONFIG -------------------------
N = 100_000_000          # numero de rectangulos (bajen este valor si sus
                          # pruebas tardan demasiado; 10^9 puede tardar minutos)
A, B = 0.0, 1000.0
THREAD_COUNTS = [1, 2, 4, 8, 16]
SCHEDULES = ["static", "dynamic", "guided"]
CHUNK = 1000
REPETICIONES = 3         # corridas por configuracion, se promedian

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SEQ_BIN = os.path.join(ROOT, "secuencial", "riemann_seq")
PAR_BIN = os.path.join(ROOT, "paralelo", "riemann_par")
OUT_CSV = os.path.join(os.path.dirname(os.path.abspath(__file__)), "resultados.csv")
OUT_PLOT_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "graficas")
# ------------------------------------------------------------


def compilar():
    print("Compilando...")
    subprocess.run(["make", "-C", ROOT], check=True)


def correr(binario, args):
    """Corre el binario y extrae el tiempo de la linea RESULT."""
    salida = subprocess.run([binario] + [str(a) for a in args],
                             capture_output=True, text=True, check=True)
    for linea in salida.stdout.splitlines():
        if linea.startswith("RESULT"):
            partes = linea.split(",")
            tiempo = float(partes[5])
            return tiempo
    raise RuntimeError(f"No se encontro linea RESULT en la salida de {binario}")


def promedio_de_corridas(binario, args, repeticiones):
    tiempos = [correr(binario, args) for _ in range(repeticiones)]
    return statistics.mean(tiempos), statistics.stdev(tiempos) if len(tiempos) > 1 else 0.0


def main():
    compilar()

    filas = []

    # --- baseline secuencial ---
    print(f"Corriendo secuencial (n={N})...")
    t_seq, sd_seq = promedio_de_corridas(SEQ_BIN, [N, A, B], REPETICIONES)
    print(f"  tiempo promedio secuencial = {t_seq:.4f}s (sd={sd_seq:.4f})")
    filas.append({
        "version": "secuencial", "threads": 1, "schedule": "-", "n": N,
        "tiempo_prom": t_seq, "tiempo_sd": sd_seq,
        "speedup": 1.0, "eficiencia": 1.0,
    })

    # --- versiones paralelas ---
    for schedule in SCHEDULES:
        for threads in THREAD_COUNTS:
            print(f"Corriendo paralelo (threads={threads}, schedule={schedule})...")
            t_par, sd_par = promedio_de_corridas(
                PAR_BIN, [N, A, B, threads, schedule, CHUNK], REPETICIONES
            )
            speedup = t_seq / t_par
            eficiencia = speedup / threads
            print(f"  tiempo={t_par:.4f}s  speedup={speedup:.2f}  eficiencia={eficiencia:.2f}")
            filas.append({
                "version": "paralelo", "threads": threads, "schedule": schedule,
                "n": N, "tiempo_prom": t_par, "tiempo_sd": sd_par,
                "speedup": speedup, "eficiencia": eficiencia,
            })

    # --- guardar CSV ---
    with open(OUT_CSV, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=list(filas[0].keys()))
        writer.writeheader()
        writer.writerows(filas)
    print(f"\nCSV guardado en: {OUT_CSV}")

    # --- graficas ---
    generar_graficas(filas)


def generar_graficas(filas):
    try:
        import matplotlib
        matplotlib.use("Agg")
        import matplotlib.pyplot as plt
    except ImportError:
        print("matplotlib no esta instalado, se omiten las graficas "
              "(pip install matplotlib --break-system-packages)")
        return

    os.makedirs(OUT_PLOT_DIR, exist_ok=True)
    schedules = sorted(set(f["schedule"] for f in filas if f["version"] == "paralelo"))

    # Speedup vs threads
    plt.figure()
    for sched in schedules:
        datos = [f for f in filas if f["version"] == "paralelo" and f["schedule"] == sched]
        datos.sort(key=lambda x: x["threads"])
        plt.plot([d["threads"] for d in datos], [d["speedup"] for d in datos],
                  marker="o", label=sched)
    max_t = max(f["threads"] for f in filas if f["version"] == "paralelo")
    plt.plot([1, max_t], [1, max_t], "k--", label="speedup ideal")
    plt.xlabel("Numero de threads")
    plt.ylabel("Speedup")
    plt.title("Speedup vs. numero de threads")
    plt.legend()
    plt.grid(True)
    plt.savefig(os.path.join(OUT_PLOT_DIR, "speedup.png"), dpi=150)
    plt.close()

    # Eficiencia vs threads
    plt.figure()
    for sched in schedules:
        datos = [f for f in filas if f["version"] == "paralelo" and f["schedule"] == sched]
        datos.sort(key=lambda x: x["threads"])
        plt.plot([d["threads"] for d in datos], [d["eficiencia"] for d in datos],
                  marker="o", label=sched)
    plt.axhline(1.0, color="k", linestyle="--", label="eficiencia ideal")
    plt.xlabel("Numero de threads")
    plt.ylabel("Eficiencia")
    plt.title("Eficiencia vs. numero de threads")
    plt.legend()
    plt.grid(True)
    plt.savefig(os.path.join(OUT_PLOT_DIR, "eficiencia.png"), dpi=150)
    plt.close()

    print(f"Graficas guardadas en: {OUT_PLOT_DIR}")


if __name__ == "__main__":
    main()
