CC = gcc
CFLAGS = -O2
LIBS = -lm

all: secuencial/riemann_seq paralelo/riemann_par

secuencial/riemann_seq: secuencial/riemann_seq.c
	$(CC) $(CFLAGS) -o secuencial/riemann_seq secuencial/riemann_seq.c $(LIBS)

paralelo/riemann_par: paralelo/riemann_par.c
	$(CC) $(CFLAGS) -fopenmp -o paralelo/riemann_par paralelo/riemann_par.c $(LIBS)

clean:
	rm -f secuencial/riemann_seq paralelo/riemann_par

.PHONY: all clean
