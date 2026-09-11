#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_N 2000

int counter;

void *worker(void *arg) {
  printf("oi da thread %ld\n", (long)arg);
  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "%s N", argv[0]);
    return 1;
  }
  char *endptr;

  long n_long = strtol(argv[1], &endptr, 10);
  if (*endptr != '\0' || n_long < 1 || n_long > MAX_N) {
    fprintf(stderr, "N deve estar entre 1 e %d", MAX_N);
    return 1;
  }
  int n = (int)n_long;

  pthread_t tid[n];
  for (int i = 0; i < n; i++) {
    int rc = pthread_create(&tid[i], NULL, worker, (void *)(long)i);
    if (rc != 0) {
      fprintf(stderr, "pthread_create falhou %d\n", rc);
      return 1;
    }
  }
  for (int i = 0; i < n; i++) {
    pthread_join(tid[i], NULL);
  }
  return 0;
}
