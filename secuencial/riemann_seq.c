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

static int parsear_long_positivo(const char *texto, long *valor) {
    char *fin;
    errno = 0;
    long convertido = strtol(texto, &fin, 10);
    if (errno != 0 || fin == texto || *fin != '\0' || convertido <= 0) {
        return 0;
    }
    *valor = convertido;
    return 1;
}

static int parsear_double_finito(const char *texto, double *valor) {
    char *fin;
    errno = 0;
    double convertido = strtod(texto, &fin);
    if (errno != 0 || fin == texto || *fin != '\0' || !isfinite(convertido)) {
        return 0;
    }
    *valor = convertido;
    return 1;
}

int main(int argc, char *argv[]) {
    long n = 1000000000L;
    double a = 0.0;
    double b = 1000.0;

    if (argc > 4 || (argc > 1 && !parsear_long_positivo(argv[1], &n)) ||
        (argc > 2 && !parsear_double_finito(argv[2], &a)) ||
        (argc > 3 && !parsear_double_finito(argv[3], &b))) {
        fprintf(stderr, "Uso: %s [n>0] [a] [b]\n", argv[0]);
        return EXIT_FAILURE;
    }

    struct timespec inicio, fin;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    double resultado = riemann_secuencial(a, b, n);

    clock_gettime(CLOCK_MONOTONIC, &fin);
    double tiempo = (fin.tv_sec - inicio.tv_sec) +
                    (fin.tv_nsec - inicio.tv_nsec) / 1e9;

    printf("n=%ld a=%.4f b=%.4f\n", n, a, b);
    printf("Area = %.10f\n", resultado);
    printf("Tiempo = %.6f s\n", tiempo);

    // Salida estable que procesa benchmark.py 
    printf("RESULT,seq,1,%ld,%.10f,%.6f\n", n, resultado, tiempo);

    return 0;
}