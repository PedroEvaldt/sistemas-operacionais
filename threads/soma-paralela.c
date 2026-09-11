#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_THREADS 2000
#define TOTAL 1000003

typedef struct {
  int id, inicio, fim;
  int *vetor;
  long parcial;
} args_t;

void *soma_fatia(void *arg) {
  args_t *a = arg;
  long s = 0;
  for (int i = a->inicio; i < a->fim; i++) {
    s += a->vetor[i];
  }
  a->parcial = s;
  printf("thread %d (tid %lu) [%d - %d] -> %ld\n", a->id,
         (unsigned long)pthread_self(), a->inicio, a->fim, a->parcial);
  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "%s N\n", argv[0]);
    return 1;
  }
  char *endptr;
  long n_long = strtol(argv[1], &endptr, 10);
  if (*endptr != '\0' || n_long < 2 || n_long > MAX_THREADS) {
    fprintf(stderr, "N tem que ser entre 2 e %d\n", MAX_THREADS);
    return 1;
  }
  int nthreads = (int)n_long;

  int *vetor = malloc(TOTAL * sizeof(int));
  for (int i = 0; i < TOTAL; i++) {
    vetor[i] = 1;
  }

  pthread_t t[nthreads];
  args_t args[nthreads];

  int tam = TOTAL / nthreads;

  for (int i = 0; i < nthreads; i++) {
    args[i] = (args_t){.id = i,
                       .inicio = i * tam,
                       .fim = (i == nthreads - 1) ? TOTAL : (i + 1) * tam,
                       .vetor = vetor};
    if (pthread_create(&t[i], NULL, soma_fatia, &args[i]) != 0) {
      perror("pthread_create");
      return 1;
    }
  }
  for (int i = 0; i < nthreads; i++) {
    pthread_join(t[i], NULL);
  }
  long total = 0;
  for (int i = 0; i < nthreads; i++) {
    total += args[i].parcial;
  }
  printf("total = %ld; esperado %d\n", total, TOTAL);
  free(vetor);
  return 0;
}
