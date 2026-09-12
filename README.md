# Universidad del Valle de Guatemala  
## Departamento de Ciencias de la Computación  
## CC3069 Computación Paralela y Distribuida 


#  Parcial 1 - Consultoría HPC  

**Problema:** 2. Integración Numérica (Suma de Riemann)


# **Consultora Apex Compute**

## Autores
- Angie Vela, 23764
- Cristian Tunchez, 231159

## Estructura del repositorio

```
.
├── secuencial/       # Algoritmo base (sin paralelizar)
│   └── riemann_seq.c
├── paralelo/         # Solución optimizada con OpenMP
│   └── riemann_par.c
├── docs/             # Reporte, gráficas y script de benchmarking
│   ├── reporte.md
│   ├── benchmark.py
│   └── graficas/
├── Makefile
└── README.md
```

## Compilar

Se requiere GCC con soporte para OpenMP, GNU Make y la biblioteca matemática
estándar. En Windows se recomienda ejecutar el proyecto dentro de WSL.

```bash
make            # compila secuencial y paralelo
make clean      # limpia binarios
```

También se puede compilar cada versión directamente desde la raíz:

```bash
gcc -O2 -o secuencial/riemann_seq secuencial/riemann_seq.c -lm
gcc -O2 -fopenmp -o paralelo/riemann_par paralelo/riemann_par.c -lm
```

## Ejecutar

Secuencial:
```bash
./secuencial/riemann_seq [n] [a] [b]
# ejemplo:
./secuencial/riemann_seq 1000000000 0 1000
```

Paralelo:
```bash
./paralelo/riemann_par [n] [a] [b] [num_threads] [schedule: static|dynamic|guided] [chunk]
# ejemplo:
./paralelo/riemann_par 1000000000 0 1000 8 static
```

Los parámetros son opcionales. `n` y el número de threads deben ser positivos;
los schedules admitidos son `static`, `dynamic` y `guided`. El argumento
`chunk` se utiliza únicamente con `dynamic` y `guided`.

La función integrada por ambas versiones es:

```text
f(x) = sin(x) cos(x) + x²
```

Como control de corrección, su integral exacta en `[a,b]` es
`(sin²(b)-sin²(a))/2 + (b³-a³)/3`. La aproximación debe acercarse a ese valor
al aumentar `n`; por el distinto orden de las sumas en punto flotante, las
versiones secuencial y paralela pueden diferir ligeramente en los últimos
dígitos.

## Benchmarking automático

Genera un CSV con tiempos, speedup y eficiencia, además de gráficas, para
distintas combinaciones de threads y schedule (editen la configuración al
inicio de `docs/benchmark.py` según lo que necesiten probar):

```bash
python3 docs/benchmark.py
```

Requiere Python 3 y Matplotlib, puede instalarse en un entorno virtual.

