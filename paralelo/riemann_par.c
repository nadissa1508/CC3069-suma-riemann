/*
 * Universidad del Valle de Guatemala
 * Departamento de Computación
 * CC3069 Computación Paralela 
 * 
 * Consultoría Apex Compute 
 * Problema 2: Integración Numérica (Suma de Riemann)
 * Versión paralela con OpenMP
 *
 * Autores:
 *   - Angie Vela, 23764
 *   - Cristian Tunchez, 231159
 *
 * Archivo: riemann_par.c
 * 
 * Descripción:
 * Aproxima la integral de f(x) en [a, b] mediante una suma de Riemann
 * con puntos medios y distribuye las n iteraciones entre threads OpenMP.
 * 
 * Estrategia de paralelización:
 * - parallel for reparte las iteraciones independientes entre threads.
 * - reduction(+:suma) crea acumuladores privados y los combina al final,
 *   evitando una condición de carrera sin sincronizar cada iteración.
 * - static es el schedule predeterminado porque todas las iteraciones tienen
 *   costo similar; dynamic y guided se conservan para comparación experimental.
 * - El chunk indicado por línea de comandos solo se usa con dynamic y guided.
 * 
 * Compilar:  gcc -O2 -fopenmp -o riemann_par riemann_par.c -lm
 * Ejecutar:  ./riemann_par [n] [a] [b] [num_threads] [schedule: static|dynamic|guided] [chunk]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <limits.h>
#include <omp.h>

// Función matemática usada por ambas versiones
double f(double x) {
    return sin(x) * cos(x) + x * x;
}

// Aplica la regla del punto medio y distribuye el ciclo según el schedule pedido
double riemann_paralelo(double a, double b, long n, int num_threads,
                         const char *schedule_tipo, int chunk) {
    // Ancho de cada rectángulo
    double h = (b - a) / (double)n;
    double suma = 0.0;

    // Impide que el runtime reduzca dinámicamente el número solicitado de threads
    omp_set_dynamic(0);
    omp_set_num_threads(num_threads);

    if (strcmp(schedule_tipo, "dynamic") == 0) {
        // La reducción combina de forma segura una suma parcial por thread
        #pragma omp parallel for reduction(+:suma) schedule(dynamic, chunk)
        for (long i = 0; i < n; i++) {
            double x_medio = a + (i + 0.5) * h;
            suma += f(x_medio) * h;
        }
    } else if (strcmp(schedule_tipo, "guided") == 0) {
        #pragma omp parallel for reduction(+:suma) schedule(guided, chunk)
        for (long i = 0; i < n; i++) {
            double x_medio = a + (i + 0.5) * h;
            suma += f(x_medio) * h;
        }
    } else { /* schedule static por defecto */
        #pragma omp parallel for reduction(+:suma) schedule(static)
        for (long i = 0; i < n; i++) {
            double x_medio = a + (i + 0.5) * h;
            suma += f(x_medio) * h;
        }
    }

    return suma;
}
