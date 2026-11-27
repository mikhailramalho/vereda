// https://gitlab.com/sosy-lab/benchmarking/sv-benchmarks
//
// SPDX-FileCopyrightText: 2005-2021 University of Tartu & Technische Universität München
//
// SPDX-License-Identifier: MIT

#include <pthread.h>
#include <stdio.h>

int myglobal;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void *t_fun(void *arg) {
  int *p = &myglobal;
  pthread_mutex_lock(&mutex1);
  *p = *p + 1; // RACE!
  pthread_mutex_unlock(&mutex1);
  return NULL;
}

int main(void) {
  pthread_t id;
  pthread_create(&id, NULL, t_fun, NULL);
  pthread_mutex_lock(&mutex2);
  myglobal=myglobal+1; // RACE!
  pthread_mutex_unlock(&mutex2);
  pthread_join (id, NULL);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_2384
// Needs: --no-por --context-bound 5 --ub-data-race-check
// RUN: --ub-div-by-zero-check=false --force-malloc-success --k-step 2 --unlimited-k-steps --vla-size-check=false -m32 --no-por --context-bound 5 --ub-pointer-check=false --ub-data-race-check --no-assertions --incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
