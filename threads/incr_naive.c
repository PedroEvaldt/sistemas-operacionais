#include <pthread.h>
#include <stdio.h>

#define NTHREAD 4
#define NINCR 100000

long contador = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *incr(void *arg) {
  for (int i = 0; i < NINCR; i++) {
    pthread_mutex_lock(&mutex);
    contador++;
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

int main(void) {
  pthread_t t[NTHREAD];
  for (int i = 0; i < NTHREAD; i++) {
    pthread_create(&t[i], NULL, incr, NULL);
  }
  for (int i = 0; i < NTHREAD; i++) {
    pthread_join(t[i], NULL);
  }
  printf("esperado: %d - obtido: %ld\n", (NTHREAD * NINCR), contador);
  return 0;
}
