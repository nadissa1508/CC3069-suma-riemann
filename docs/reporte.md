# Reporte — Problema 2: Integración Numérica (Suma de Riemann)

## 1. Contexto y datos

El problema consiste en aproximar la integral definida de una función en el
intervalo `[a,b]`. Se usa la regla compuesta del punto medio: el intervalo se
divide en `n` partes de ancho `h=(b-a)/n` y se suma el área de los rectángulos:

```text
suma = Σ f(a + (i + 0.5)h) h, para i = 0, ..., n-1
```

La implementación evalúa `f(x)=sin(x)cos(x)+x²`. Su integral exacta, útil como
referencia de corrección, es
`(sin²(b)-sin²(a))/2 + (b³-a³)/3`.

- Algoritmo secuencial: un ciclo recorre y acumula las `n` contribuciones.
- Origen de los datos: los valores `n`, `a` y `b` se reciben por línea de
  comandos o usan los valores predeterminados `10^9`, `0` y `1000`.
- Tamaño de muestra elegido: [PENDIENTE: indicar cada valor de `n` probado].
- Justificación de la muestra: [PENDIENTE: explicar por qué los tamaños elegidos
  producen tiempos medibles y representan cargas pequeña, mediana y grande].
- Memoria: no se almacenan los rectángulos. Se usan variables escalares para
  `h`, `suma`, el índice y el punto medio; la memoria auxiliar es O(1) en la
  versión secuencial y O(P) para las sumas privadas administradas por OpenMP.

## 2. Estrategia de paralelización

La directiva `#pragma omp parallel for reduction(+:suma)` crea un equipo de
threads, reparte las iteraciones independientes y asigna a cada thread un
acumulador privado. Al finalizar el ciclo, OpenMP combina las sumas parciales.
Esto evita la condición de carrera que existiría si todos actualizaran `suma`
directamente, sin el costo de ejecutar `critical` o `atomic` en cada iteración.

El programa desactiva el ajuste dinámico de OpenMP y solicita mediante
`omp_set_num_threads` el número indicado por línea de comandos. Se ofrecen:

- `static`: política predeterminada. Es la candidata principal porque todas las
  iteraciones evalúan la misma expresión y tienen un costo aproximadamente igual.
- `dynamic`: entrega bloques conforme los threads terminan. Puede mejorar cargas
  desbalanceadas, pero aquí normalmente añade overhead.
- `guided`: empieza con bloques grandes y los reduce progresivamente; también se
  incluye para comparación experimental.

El argumento `chunk` solo configura el tamaño mínimo/de bloque en `dynamic` y
`guided`. La rama `static` usa `schedule(static)` sin chunk explícito.

## 3. Metodología de pruebas

Cada integrante debe ejecutar el protocolo completo en su propia computadora:

1. Registrar nombre del integrante, modelo de CPU, núcleos físicos/lógicos,
   sistema operativo y compilador.
2. Cerrar aplicaciones pesadas y mantener condiciones similares entre corridas.
3. Compilar una sola vez con `make`.
4. Hacer una corrida corta de calentamiento que no se registre.
5. Elegir el mismo `n`, `a` y `b` para la versión secuencial y todas las paralelas.
6. Ejecutar al menos cinco repeticiones por configuración y reportar promedio y
   desviación estándar (o mediana, si se justifica y se usa consistentemente).
7. Probar `P=1,2,4,...` hasta el número de threads lógicos del equipo. No es
   necesario probar 16 si el equipo no dispone de 16 threads lógicos.
8. Usar `static` como comparación principal. Comparar además `dynamic` y `guided`
   con el mismo chunk; si se estudian varios chunks, cambiar solo ese parámetro.
9. Confirmar que las áreas secuencial y paralela coincidan dentro de una
   tolerancia razonable; pequeñas diferencias finales son normales por el orden
   de las operaciones de punto flotante.
10. Capturar terminal, parámetros y salidas como evidencia. No medir el tiempo de
    compilación ni combinar resultados de máquinas distintas.

Valores sugeridos para explorar antes de elegir la muestra final: `n=10^6`,
`10^7`, `10^8` y, si el tiempo es razonable, `10^9`. Para la tabla definitiva se
debe escoger un `n` suficientemente grande para que el trabajo domine el overhead
de OpenMP y mantenerlo fijo en la comparación de threads/schedules.

Comandos de ejemplo desde la raíz:

```bash
make
./secuencial/riemann_seq 100000000 0 1000
./paralelo/riemann_par 100000000 0 1000 4 static
./paralelo/riemann_par 100000000 0 1000 4 dynamic 1000
./paralelo/riemann_par 100000000 0 1000 4 guided 1000
python3 docs/benchmark.py
```

## 4. Resultados y métricas individuales

Para cada integrante y cada configuración:

```text
Speedup S = T_secuencial / T_paralelo
Eficiencia E = S / P
```

`T_secuencial` es el tiempo promedio de la versión secuencial;
`T_paralelo`, el promedio de la versión paralela; y `P`, el número de threads de
esa corrida. La eficiencia puede expresarse como fracción o como porcentaje
`100E%`, indicando cuál formato se usa.

### Integrante: Angie Vela

Equipo/CPU: 
  - Procesador 11th Gen Intel(R) Core(TM) i7-11800H @ 2.30GHz (2.30 GHz) 
  - RAM 16.0 GB
Sistema/compilador:
  - Windows 11 Home Single Language

| Integrante | Equipo/CPU | n | Threads | Schedule | Chunk | T secuencial (s) | T paralelo (s) | Speedup | Eficiencia |
|---|---|---:|---:|---|---:|---:|---:|---:|---:|
| Angie Vela | [PENDIENTE] | [PENDIENTE] | 1 | static | N/A | [PENDIENTE] | [PENDIENTE] | [CALCULAR] | [CALCULAR] |

### Integrante: Cristian Tunchez

Equipo/CPU: [PENDIENTE]  
Sistema/compilador: [PENDIENTE]

| Integrante | Equipo/CPU | n | Threads | Schedule | Chunk | T secuencial (s) | T paralelo (s) | Speedup | Eficiencia |
|---|---|---:|---:|---|---:|---:|---:|---:|---:|
| Cristian Tunchez | [PENDIENTE] | [PENDIENTE] | 1 | static | N/A | [PENDIENTE] | [PENDIENTE] | [CALCULAR] | [CALCULAR] |

Para cada integrante se recomienda incluir:

- una tabla completa y una gráfica de tiempo contra threads;
- una gráfica de speedup con la línea ideal `S=P`;
- una gráfica de eficiencia contra threads;
- capturas de corridas representativas con comandos y salidas visibles;
- un análisis de tendencias, anomalías, overhead y relación con núcleos físicos.

## 5. Conclusiones

[PENDIENTE: resumir con base en mediciones reales cuándo la paralelización fue
útil, qué configuración funcionó mejor y por qué la eficiencia cambió con P].
