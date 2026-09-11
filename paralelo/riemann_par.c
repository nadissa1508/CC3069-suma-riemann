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

static int parsear_int_positivo(const char *texto, int *valor) {
    long convertido;
    if (!parsear_long_positivo(texto, &convertido) || convertido > INT_MAX) {
        return 0;
    }
    *valor = (int)convertido;
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

static int schedule_valido(const char *schedule_tipo) {
    return strcmp(schedule_tipo, "static") == 0 ||
           strcmp(schedule_tipo, "dynamic") == 0 ||
           strcmp(schedule_tipo, "guided") == 0;
}

int main(int argc, char *argv[]) {
    long n = 1000000000L;
    double a = 0.0;
    double b = 1000.0;
    int num_threads = omp_get_max_threads();
    const char *sched = (argc > 5) ? argv[5] : "static";
    int chunk = 1000;

    if (argc > 7 || (argc > 1 && !parsear_long_positivo(argv[1], &n)) ||
        (argc > 2 && !parsear_double_finito(argv[2], &a)) ||
        (argc > 3 && !parsear_double_finito(argv[3], &b)) ||
        (argc > 4 && !parsear_int_positivo(argv[4], &num_threads)) ||
        !schedule_valido(sched) ||
        (argc > 6 && !parsear_int_positivo(argv[6], &chunk))) {
        fprintf(stderr,
                "Uso: %s [n>0] [a] [b] [threads>0] "
                "[static|dynamic|guided] [chunk>0]\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    double inicio = omp_get_wtime();
    double resultado = riemann_paralelo(a, b, n, num_threads, sched, chunk);
    double fin = omp_get_wtime();
    double tiempo = fin - inicio;

    printf("n=%ld a=%.4f b=%.4f threads=%d schedule=%s chunk=%d%s\n",
           n, a, b, num_threads, sched, chunk,
           strcmp(sched, "static") == 0 ? " (no se usa con static)" : "");
    printf("Area = %.10f\n", resultado);
    printf("Tiempo = %.6f s\n", tiempo);

    // Salida estable que procesa benchmark.py 
    printf("RESULT,par,%d,%ld,%.10f,%.6f,%s,%d\n",
           num_threads, n, resultado, tiempo, sched, chunk);

    return 0;
}
