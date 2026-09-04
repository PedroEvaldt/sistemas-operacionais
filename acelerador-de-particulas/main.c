#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_N 1000
#define DEBUG 0

void debug() {
  fflush(stdout);
  char cmd[64];
  snprintf(cmd, sizeof cmd, "ls -l /proc/%d/fd", getpid());
  system(cmd);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "%s N\n", argv[0]);
    return 1;
  }

  char *endptr;
  long n_long = strtol(argv[1], &endptr, 10);
  if (*endptr != '\0' || n_long < 2 || n_long > MAX_N) {
    fprintf(stderr, "N tem que estar entre 2 e %d\n", MAX_N);
    return 1;
  }

  int n = (int)n_long;
  int pipes_r[n][2];
  int pipes_l[n][2];

  // Criacao dos pipes passando os file descriptors
  for (int i = 0; i < n; i++) {
    int return_pipe_r = pipe(pipes_r[i]);
    int return_pipe_l = pipe(pipes_l[i]);
    if (return_pipe_l != 0 || return_pipe_r != 0) {
      perror("pipe");
      return 1;
    }
  }

  // Parte central do sistema, colocando cada ponta, pedindo fork, e fechando
  // file descriptors que nao serao usados
  for (int i = 0; i < n; i++) {
    int read_r = pipes_r[(i + n - 1) % n][0];
    int write_r = pipes_r[i][1];
    int read_l = pipes_l[i][0];
    int write_l = pipes_l[(i + n - 1) % n][1];
    pid_t pid = fork();
    if (pid < 0) {
      perror("fork");
      exit(1);
    }
    if (pid == 0) {

      for (int j = 0; j < n; j++) {
        if (pipes_r[j][0] != read_r)
          close(pipes_r[j][0]);
        if (pipes_r[j][1] != write_r)
          close(pipes_r[j][1]);
        if (pipes_l[j][0] != read_l)
          close(pipes_l[j][0]);
        if (pipes_l[j][1] != write_l)
          close(pipes_l[j][1]);
      }
      uint8_t particula;
      if (i == 0) {
        particula = 1;
        write(write_r, &particula, 1);
        write(write_l, &particula, 1);
      }
      for (;;) {
        // Sentido horario
        ssize_t rr = read(read_r, &particula, 1);
        if (rr == 0) {
          fprintf(stderr, "filho %d: EOF\n", i);
          break;
        }
        if (rr < 0) {
          perror("read");
          break;
        }
        printf("filho %d Recebeu [HORARIO]\n", i);
        usleep(500000);
        write(write_r, &particula, 1);

        // Sentido anti-horario
        ssize_t rl = read(read_l, &particula, 1);
        if (rl == 0) {
          fprintf(stderr, "filho %d: EOF\n", i);
          break;
        }
        if (rl < 0) {
          perror("read");
          break;
        }
        printf("filho %d Recebeu [ANTI-HORARIO]\n", i);
        usleep(500000);
        write(write_l, &particula, 1);
      }
      if (DEBUG)
        debug();
      _exit(0);
    }
  }
  for (int i = 0; i < n; i++) {
    close(pipes_r[i][0]);
    close(pipes_r[i][1]);
    close(pipes_l[i][0]);
    close(pipes_l[i][1]);
  }

  for (int i = 0; i < n; i++) {
    int st;
    pid_t p = waitpid(-1, &st, 0);
    if (p < 0) {
      perror("waitpid");
      break;
    }
    if (WIFEXITED(st)) {
      fprintf(stderr, "filho %d saiu com codigo %d\n", p, WEXITSTATUS(st));
    } else if (WIFSIGNALED(st)) {
      fprintf(stderr, "filho %d morto por sinal %d\n", p, WTERMSIG(st));
    }
  }
}
