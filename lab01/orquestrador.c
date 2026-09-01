/*
 * orquestrador.c — Cadeia de processos com fork() + pipe() + exec()
 *
 * INF01142 — Sistemas Operacionais — 2026/2
 *
 * Grupo:
 *
 * Integrante 1: Pedro Evaldt — Cartão UFRGS: 599076 — pedrofossati067@gmail.com
 * Integrante 2: Gabriel Panerai — Cartão UFRGS: 598731 —
 * gabrielpanerai@gmail.com
 *
 * =========================================================================
 * ESTE ARQUIVO (junto com relay.c) É O QUE VOCÊ DEVE IMPLEMENTAR.
 * =========================================================================
 *
 * A validação de argv já está pronta abaixo. Falta implementar o laço
 * principal que monta a cadeia de N processos relay, conectados por
 * pipes anônimos, e a espera final pelos filhos.
 *
 * Chamadas de sistema que você vai precisar:
 *   open(2), pipe(2), fork(2), dup2(2), execv(3), close(2), waitpid(2)
 *
 * Referências rápidas:
 *   man 2 open     man 2 pipe    man 2 fork
 *   man 2 dup2     man 3 execv   man 2 waitpid
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_N 100

int main(int argc, char *argv[]) {
  // Verifica se o programa foi chamado com 2 argumentos
  if (argc != 2) {
    fprintf(stderr, "uso: %s N\n", argv[0]);
    return 1;
  }

  // Converte o argumento passado para a variavel N
  char *endptr;
  long n_long = strtol(argv[1], &endptr, 10);
  if (*endptr != '\0' || n_long < 1 || n_long > MAX_N) {
    fprintf(stderr, "N deve ser um inteiro entre 1 e %d\n", MAX_N);
    return 1;
  }
  int n = (int)n_long;

  // Le de /dev/null e passa como primeira entrada
  int devnull_fd = open("/dev/null", O_RDONLY);
  int prev_read = devnull_fd;
  pid_t pids[MAX_N];

  for (int i = 0; i < n; i++) {
    int fd[2];
    if (i < n - 1) {
      pipe(fd);
    }
    pid_t pid = fork();
    if (pid == 0) {
      dup2(prev_read, STDIN_FILENO);
      close(prev_read);
      if (i < n - 1) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
      }
      execv("./relay", (char *[]){"./relay", NULL});
      perror("DEU RUIM");
      exit(1);
    } else {
      pids[i] = pid;
      close(prev_read);
      if (i < n - 1) {
        close(fd[1]);
        prev_read = fd[0];
      }
    }
  }
  for (int i = 0; i < n; i++) {
    int status;
    waitpid(pids[i], &status, 0);
    fprintf(stderr, "PID=[%d] - STATUS=[%d]\n", pids[i], status);
  }
  return 0;
}
