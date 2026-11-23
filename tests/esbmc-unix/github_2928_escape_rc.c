// This file is part of the SV-Benchmarks collection of verification tasks:
// https://gitlab.com/sosy-lab/benchmarking/sv-benchmarks
//
// SPDX-FileCopyrightText: 2005-2021 University of Tartu & Technische Universität München
//
// SPDX-License-Identifier: MIT

#include <pthread.h>
#include <stdio.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void *t_fun(void *arg) {
  int *p = (int *) arg;
  pthread_mutex_lock(&mutex1);
  (*p)++; // RACE!
  pthread_mutex_unlock(&mutex1);
  return NULL;
}

int main(void) {
  pthread_t id;
  int i;
  pthread_create(&id, NULL, t_fun, (void *) &i);
  pthread_mutex_lock(&mutex2);
  i++; // RACE!
  pthread_mutex_unlock(&mutex2);
  pthread_join (id, NULL);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_2928_escape_rc
// Needs: --sv-comp --force-realloc-success --state-hashing --add-symex-value-sets --smt-symex-guard --cswitch-skip-readonly-globals --data-races-check-only --no-assertions REQUIRES arch32
// RUN: --32 --sv-comp --no-div-by-zero-check --force-malloc-success --force-realloc-success --state-hashing --add-symex-value-sets --smt-symex-guard --cswitch-skip-readonly-globals --no-pointer-check --no-bounds-check --data-races-check-only --no-assertions --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^.*data race.*$
// Vereda: verdict none
