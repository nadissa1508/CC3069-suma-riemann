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
  - 8 nucleos, 16 hilos
  - RAM 16.0 GB
Sistema/compilador:
  - Windows 11 Home Single Language

#### Configuración experimental

Las mediciones de Nadissa se realizaron con `n=200,000,000` rectángulos en el
intervalo `[0,1000]`. Se probaron `1`, `2`, `4`, `8` y `16` threads con los
schedules `static`, `dynamic` y `guided`. Para `dynamic` y `guided` se utilizó un
chunk de `1000`; en `static` este parámetro no se usa. Cada valor de la tabla es
el promedio de tres repeticiones y se acompaña con su desviación estándar. Las
capturas documentan la ejecución en WSL Ubuntu 20.04; el modelo de CPU no está
registrado en las evidencias disponibles y debe agregarse antes de la entrega.
Los valores originales, sin redondear, se conservan en
[`resultados_nadissa.csv`](resultados_nadissa.csv).

#### Resultados

| Versión | Threads | Schedule | Chunk | Tiempo promedio (s) | SD (s) | Speedup | Eficiencia |
|---|---:|---|---:|---:|---:|---:|---:|
| Secuencial | 1 | — | — | 3.6895 | 0.1339 | 1.0000x | 100.00% |
| Paralela | 1 | static | N/A | 3.8586 | 0.5101 | 0.9562x | 95.62% |
| Paralela | 2 | static | N/A | 2.0698 | 0.0585 | 1.7826x | 89.13% |
| Paralela | 4 | static | N/A | 1.1110 | 0.1229 | 3.3209x | 83.02% |
| Paralela | 8 | static | N/A | 0.7275 | 0.0287 | 5.0716x | 63.40% |
| Paralela | 16 | static | N/A | 0.5480 | 0.0427 | 6.7327x | 42.08% |
| Paralela | 1 | dynamic | 1000 | 4.7665 | 0.6043 | 0.7740x | 77.40% |
| Paralela | 2 | dynamic | 1000 | 2.4835 | 0.2138 | 1.4856x | 74.28% |
| Paralela | 4 | dynamic | 1000 | 1.2573 | 0.0982 | 2.9344x | 73.36% |
| Paralela | 8 | dynamic | 1000 | 0.8110 | 0.0584 | 4.5491x | 56.86% |
| Paralela | 16 | dynamic | 1000 | 0.6112 | 0.0198 | 6.0370x | 37.73% |
| Paralela | 1 | guided | 1000 | 4.5700 | 0.5612 | 0.8073x | 80.73% |
| Paralela | 2 | guided | 1000 | 1.8801 | 0.2159 | 1.9624x | 98.12% |
| Paralela | 4 | guided | 1000 | 1.0658 | 0.0565 | 3.4619x | 86.55% |
| Paralela | 8 | guided | 1000 | 0.6737 | 0.0274 | 5.4763x | 68.45% |
| Paralela | 16 | guided | 1000 | 0.4573 | 0.0180 | 8.0677x | 50.42% |

El tiempo secuencial promedio de `3.6895 s` funciona como línea base para
calcular el speedup y la eficiencia de todas las configuraciones paralelas de
Nadissa. Esto mantiene la comparación en el mismo entorno y con el mismo tamaño
del problema.

#### Escalabilidad y comparación de schedules

Con `schedule(static)`, un thread obtuvo un speedup de `0.9562x`, por lo que no
superó la línea base secuencial. La diferencia es coherente con el overhead de
crear y administrar la región paralela y efectuar la reducción cuando no existe
paralelismo efectivo. A partir de dos threads aparece una mejora clara: el
speedup aumenta a `1.7826x`, `3.3209x`, `5.0716x` y `6.7327x` al utilizar 2, 4,
8 y 16 threads, respectivamente.

El crecimiento no es lineal. Aunque el tiempo continúa disminuyendo, la
eficiencia de `static` baja de `89.13%` con dos threads a `42.08%` con 16. Esto
no representa un error: al aumentar `P`, el overhead de OpenMP, la reducción y
los límites de los recursos compartidos adquieren mayor peso respecto del
trabajo útil. La Figura 1 muestra esta separación progresiva respecto del
speedup ideal, mientras que la Figura 2 presenta la disminución de eficiencia.

![Speedup de las pruebas de Nadissa](graficas/speedup_nadissa.png)

*Figura 1. Speedup medido para los schedules static, dynamic y guided.*

![Eficiencia de las pruebas de Nadissa](graficas/eficiencia_nadissa.png)

*Figura 2. Eficiencia medida en función del número de threads.*

La planificación influyó en el rendimiento observado. Con ocho threads,
`guided` alcanzó `5.4763x`, frente a `5.0716x` de `static` y `4.5491x` de
`dynamic`. Con 16 threads, los resultados fueron `8.0677x` y `50.42%` para
`guided`, `6.7327x` y `42.08%` para `static`, y `6.0370x` y `37.73%` para
`dynamic`. Por tanto, `guided` produjo el mejor resultado global de estas
corridas, especialmente con 16 threads. Este resultado es experimental y no
implica que `guided` sea siempre superior en otros equipos o problemas.

En esta integración, todas las iteraciones tienen un costo computacional muy
similar. La asignación frecuente de bloques de `dynamic` introduce overhead y
no necesariamente aporta una ventaja de balance de carga; en estas mediciones
fue el schedule con menor speedup para cada cantidad de threads. Las decisiones
que hicieron efectiva la mejora sobre el algoritmo secuencial fueron repartir
las iteraciones independientes, combinar las sumas parciales mediante
`reduction` y evaluar empíricamente la política de planificación, sin cambiar la
aproximación matemática.

#### Mejor configuración y variabilidad

La mejor configuración observada fue `guided` con 16 threads: redujo el tiempo
promedio de `3.6895 s` a `0.4573 s`, obtuvo un speedup de `8.0677x` y una
eficiencia de `50.42%`. Esto equivale a una reducción aproximada del `87.60%`
del tiempo frente a la línea base y constituye la mayor aceleración medida en
los experimentos de Nadissa.

Las desviaciones estándar muestran que hubo variación entre repeticiones, razón
por la cual se reporta el promedio. Entre las configuraciones de mayor número de
threads, `guided` con 16 obtuvo una SD de `0.0180 s`, `dynamic` con 16 una de
`0.0198 s`, `static` con 8 una de `0.0287 s` y `static` con 16 una de `0.0427 s`.
Estos valores describen la dispersión observada, sin asumir que el mismo nivel de
variabilidad se repetirá en otros entornos.

#### Evidencia de ejecución y validación funcional

Las Figuras 3 y 4 muestran la ejecución real del benchmark, incluyendo la línea
base, configuraciones de los tres schedules y la generación del CSV y las
gráficas. La Figura 5 corresponde a una comprobación directa con `n=1,000,000`:
las áreas secuencial (`333333333.6751188040`) y paralela
(`333333333.6751126647`) son prácticamente iguales. La pequeña diferencia en
las últimas cifras es esperable por el orden distinto de acumulación en punto
flotante de la reducción paralela y no indica un error de implementación.

![Inicio y resultados del benchmark de Nadissa](capturas/benchmark1_nadissa.png)

*Figura 3. Línea base y primeras configuraciones registradas por el benchmark.*

![Finalización del benchmark de Nadissa](capturas/benchmark2_nadissa.png)

*Figura 4. Resultados de guided y generación de los artefactos del benchmark.*

![Validación funcional secuencial y paralela](capturas/funcionamiento%20scripts_nadissa.png)

*Figura 5. Comparación funcional de las áreas producidas por ambas versiones.*

#### Respuesta resumida

- **¿La paralelización mejoró el algoritmo?** Sí. Para dos o más threads redujo
  claramente el tiempo, y la mejor corrida pasó de `3.6895 s` a `0.4573 s`.
- **¿Cuál fue el mejor speedup?** `8.0677x`.
- **¿Cuál fue la mejor configuración?** 16 threads, `schedule(guided)` y chunk
  `1000`, con eficiencia de `50.42%`.
- **¿Qué pasó con la eficiencia al aumentar threads?** En general disminuyó al
  crecer `P`, aunque el speedup siguió aumentando; es un efecto esperado del
  overhead y del escalamiento no lineal.
- **¿Qué schedule tuvo mejor desempeño?** `guided` obtuvo el mejor desempeño en
  estas pruebas, sin que ello implique superioridad universal.


### Integrante: Cristian Tunchez

Equipo/CPU:
- AMD Ryzen 5 3500U with Radeon Vega Mobile Gfx (2.10 GHz)
- 4 núcleos, 8 hilos
- RAM 8.0 GB
Sistema/compilador:
- Windows 11 Home Single Language

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
