#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define HIST_SIZE 10

long hist[HIST_SIZE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
  int inicio;
  int fim;
  int *vetor;
} t_args;

void *errada(void *arg) {
  t_args *args = arg;
  for (int i = args->inicio; i < args->fim; i++) {
    hist[args->vetor[i] / 100]++;
  }
  return NULL;
}

void *grosso(void *arg) {
  t_args *args = arg;
  for (int i = args->inicio; i < args->fim; i++) {
    pthread_mutex_lock(&mutex);
    hist[args->vetor[i] / 100]++;
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

void *local(void *arg) {
  t_args *args = arg;
  long local[HIST_SIZE] = {0};
  for (int i = args->inicio; i < args->fim; i++) {
    local[args->vetor[i] / 100]++;
  }

  pthread_mutex_lock(&mutex);
  for (int i = 0; i < HIST_SIZE; i++) {
    hist[i] += local[i];
  }
  pthread_mutex_unlock(&mutex);
  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stderr, "%s N T modo\n", argv[0]);
    return 1;
  }

  char *endptr;
  long n_long = strtol(argv[1], &endptr, 10);
  if (*endptr != '\0' || n_long < 0) {
    fprintf(stderr, "N tem que ser um numero maior que 0\n");
    return 1;
  }
  long t_long = strtol(argv[2], &endptr, 10);
  if (*endptr != '\0' || t_long < 0 || t_long > n_long) {
    fprintf(stderr, "V tem que ser maior que zero e menor que N\n");
    return 1;
  }
  long modo_long = strtol(argv[3], &endptr, 10);
  if (*endptr != '\0' || modo_long > 2 || modo_long < 0) {
    fprintf(stderr, "modo tem que ser um 0, 1 ou 2\n");
    return 1;
  }
  int N = (int)n_long;
  int T = (int)t_long;
  int modo = (int)modo_long;

  srand(42);
  int *vetor = malloc(N * sizeof(int));
  if (vetor == NULL) {
    perror("malloc");
    return 1;
  }
  for (int i = 0; i < N; i++) {
    vetor[i] = rand() % 1000;
  }

  for (int i = 0; i < HIST_SIZE; i++) {
    hist[i] = 0;
  }

  int tam = N / T;

  pthread_t t[T];
  t_args args[T];

  switch (modo) {
  case 0:
    for (int i = 0; i < T; i++) {
      args[i] = (t_args){.inicio = i * tam,
                         .fim = (i == T - 1) ? N : (i + 1) * tam,
                         .vetor = vetor};
      if (pthread_create(&t[i], NULL, errada, &args[i]) != 0) {
        perror("pthread_create");
        return (1);
      };
    }
    break;
  case 1:
    for (int i = 0; i < T; i++) {

      args[i] = (t_args){.inicio = i * tam,
                         .fim = (i == T - 1) ? N : (i + 1) * tam,
                         .vetor = vetor};
      if (pthread_create(&t[i], NULL, grosso, &args[i]) != 0) {
        perror("pthread_create");
        return (1);
      };
    }
    break;
  case 2:
    for (int i = 0; i < T; i++) {

      args[i] = (t_args){.inicio = i * tam,
                         .fim = (i == T - 1) ? N : (i + 1) * tam,
                         .vetor = vetor};
      if (pthread_create(&t[i], NULL, local, &args[i]) != 0) {
        perror("pthread_create");
        return (1);
      };
    }
    break;
  default:
    break;
  }
  for (int i = 0; i < T; i++) {
    pthread_join(t[i], NULL);
  }

  long obtido = 0;
  for (int i = 0; i < HIST_SIZE; i++) {
    obtido += hist[i];
  }
  printf("%s --- esperado: %d; obtido: %ld\n",
         (N == obtido) ? "SUCESSO!" : "FRACASSO", N, obtido);
  free(vetor);
  return 0;
}
