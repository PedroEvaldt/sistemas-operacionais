
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int id, inicio, fim;
  long parcial, maior;
  int *vetor;
} args_t;

void *funcao(void *arg) {
  args_t *a = arg;
  long soma = 0, maior = 0;
  long qnt_pares = 0;
  for (int i = a->inicio; i < a->fim; i++) {
    if (a->vetor[i] > maior) {
      maior = (long)a->vetor[i];
    }
    if (a->vetor[i] % 2 == 0) {
      qnt_pares++;
    }
    soma += a->vetor[i];
  }
  a->parcial = soma;
  a->maior = maior;
  printf("thread %d (tid %lu) [%d - %d] -> MAIOR: %ld - SOMA: %ld\n", a->id,
         (unsigned long)pthread_self(), a->inicio, a->fim, a->maior,
         a->parcial);
  long *pares = malloc(sizeof(long));
  *pares = qnt_pares;
  return pares;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "%s N T\n", argv[0]);
    return 1;
  }

  char *endptr;
  long n_long = strtol(argv[1], &endptr, 10);
  if (n_long < 1 || *endptr != '\0') {
    fprintf(stderr, "N deve ser maior que 1\n");
    return 1;
  }
  long v_long = strtol(argv[2], &endptr, 10);
  if (v_long > n_long || *endptr != '\0') {
    fprintf(stderr, "N deve ser maior que V\n");
    return 1;
  }

  int tam_vetor = (int)n_long;
  int num_threads = (int)v_long;
  int tam = tam_vetor / num_threads;

  args_t args[num_threads];
  pthread_t threads[num_threads];
  long qnt_de_pares[num_threads];
  for (int i = 0; i < num_threads; i++) {
    qnt_de_pares[i] = 0;
  }

  srand(42);

  int *vetor = malloc(tam_vetor * sizeof(int));
  if (vetor == NULL) {
    perror("maloc");
    return 1;
  }
  for (int i = 0; i < tam_vetor; i++) {
    vetor[i] = rand() % 1000;
  }
  for (int i = 0; i < num_threads; i++) {
    args[i] = (args_t){
        .id = i,
        .inicio = i * tam,
        .fim = (i == num_threads - 1) ? tam_vetor : (i + 1) * tam,
        .vetor = vetor,
    };
    if (pthread_create(&threads[i], NULL, funcao, &args[i]) != 0) {
      perror("pthread_create");
      return 1;
    }
  }
  for (int i = 0; i < num_threads; i++) {
    void *ret;
    pthread_join(threads[i], &ret);
    qnt_de_pares[i] = *(long *)ret;
    free(ret);
  }
  long soma_recebida = 0, soma_esperada = 0, qnt_de_pares_recebida = 0,
       qnt_de_pares_esperada = 0, maior_recebido = 0, maior_esperado = 0;
  for (int i = 0; i < num_threads; i++) {
    soma_recebida += args[i].parcial;
    qnt_de_pares_recebida += qnt_de_pares[i];
    if (args[i].maior > maior_recebido) {
      maior_recebido = args[i].maior;
    }
  }
  for (int i = 0; i < tam_vetor; i++) {
    soma_esperada += vetor[i];
    if (vetor[i] % 2 == 0) {
      qnt_de_pares_esperada++;
    }
    if (vetor[i] > maior_esperado) {
      maior_esperado = vetor[i];
    }
  }
  printf("soma: receb %ld - esper %ld\nqnt pares: receb %ld - esper "
         "%ld\nmaior: receb %ld - esper %ld\n",
         soma_recebida, soma_esperada, qnt_de_pares_recebida,
         qnt_de_pares_esperada, maior_recebido, maior_esperado);

  free(vetor);
  return 0;
}
