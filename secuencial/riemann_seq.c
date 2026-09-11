/*
 * Universidad del Valle de Guatemala
 * Departamento de Computación
 * CC3069 Computación Paralela 
 * 
 * Consultoría Apex Compute
 * Problema 2: Integración Numérica (Suma de Riemann)
 * Versión secuencial
 *
 * Autores:
 *   - Angie Vela, 23764
 *   - Cristian Tunchez, 231159
 *
 * Archivo: riemann_seq.c
 * 
 * Descripción:
 * Aproxima la integral de f(x) en [a, b] mediante una suma de Riemann
 * con puntos medios. Las n contribuciones se calculan y acumulan en un
 * único flujo de ejecución.
 * 
 * Compilar:  gcc -O2 -o riemann_seq riemann_seq.c -lm
 * Ejecutar:  ./riemann_seq [n] [a] [b]
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <errno.h>

// Función matemática usada por ambas versiones 
double f(double x) {
    return sin(x) * cos(x) + x * x;
}

// Aplica la regla compuesta del punto medio con n subintervalos uniformes
double riemann_secuencial(double a, double b, long n) {
    // Ancho de cada rectángulo
    double h = (b - a) / (double)n;
    double suma = 0.0;

    for (long i = 0; i < n; i++) {
        // preferimos evaluar en el punto medio que usar un extremo (es más preciso)
        double x_medio = a + (i + 0.5) * h;
        suma += f(x_medio) * h;
    }
    return suma;
}
