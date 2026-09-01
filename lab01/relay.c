/*
 * relay.c — Um estágio da cadeia
 9*
 * INF01142 — Sistemas Operacionais — 2026/2
 *
 * Grupo:
 *
 * Integrante 1: <nome completo> — Cartão UFRGS: <número> — <email>
 * Integrante 2: <nome completo> — Cartão UFRGS: <número> — <email>
 *
 * =========================================================================
 * ESTE ARQUIVO (junto com orquestrador.c) É O QUE VOCÊ DEVE IMPLEMENTAR.
 * =========================================================================
 *
 * O relay não recebe argumentos e não sabe se é o primeiro, o último
 * ou um estágio intermediário da cadeia — quem decide isso é o
 * orquestrador, através dos redirecionamentos que faz antes do
 * exec(). A função timestamp_str() já vem pronta abaixo: use-a, não
 * precisa reimplementá-la.
 *
 * Chamadas de sistema que você vai precisar:
 *   read(2), write(2), getpid(2)
 *
 * Referências rápidas:
 *   man 2 read   man 2 write   man 2 getpid
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define INITIAL_CAP 4096

/* Escreve em buf (de tamanho n) o horario atual no formato
 * HH:MM:SS.uuuuuu (microssegundos). Fornecido pronto: os alunos nao
 * precisam implementar formatacao de horario. */
void timestamp_str(char *buf, size_t n) {
  struct timeval tv;
  gettimeofday(&tv, NULL);

  struct tm tm_info;
  localtime_r(&tv.tv_sec, &tm_info);

  char time_part[16];
  strftime(time_part, sizeof(time_part), "%H:%M:%S", &tm_info);

  int usec = (int)tv.tv_usec;
  snprintf(buf, n, "%s.%06d", time_part, usec);
}

char *read_all_stdin(size_t *out_len) {
  size_t cap = 4096;
  size_t len = 0;
  char *buf = malloc(cap);
  if (buf == NULL) {
    return NULL;
  }
  for (;;) {
    if (len == cap) {
      size_t nova_cap = cap * 2;
      char *tmp = realloc(buf, nova_cap);
      if (tmp == NULL) {
        free(buf);
        return NULL;
      }
      buf = tmp;
      cap = nova_cap;
    }
    ssize_t n = read(STDIN_FILENO, buf + len, cap - len);
    if (n == 0) {
      break;
    }
    if (n < 0) {
      if (errno == EINTR)
        continue;
      free(buf);
      return NULL;
    }
    len += (size_t)n;
  }
  if (out_len != NULL)
    *out_len = len;
  return buf;
}

void write_all(const char *buf, size_t len) {
  size_t written = 0;
  while (written < len) {
    ssize_t n = write(STDOUT_FILENO, buf + written, len - written);
    if (n == 0) {
      if (errno == EINTR)
        continue;
      return;
    }
    written += (size_t)n;
  }
  return;
}

int main(void) {
  size_t len;
  char *buf = read_all_stdin(&len);
  if (buf == NULL) {
    return 1;
  }
  char ts[24];
  timestamp_str(ts, sizeof(ts));

  char line[64];
  int line_len = snprintf(line, sizeof(line), "PID %d @ %s\n", getpid(), ts);
  if (line_len < 0) {
    free(buf);
    return 1;
  }
  size_t new_len = len + (size_t)line_len;
  char *tmp = realloc(buf, new_len);
  if (tmp == NULL) {
    free(buf);
    return 1;
  }
  buf = tmp;
  memcpy(buf + len, line, (size_t)line_len);
  write_all(buf, new_len);
  free(buf);
  return 0;
}
