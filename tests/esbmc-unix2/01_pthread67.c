//  Created by Fatimah Aljaafari on 14/06/2020.
//  Copyright © 2020 Fatimah Aljaafari. All rights reserved.
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#define N 2
char __VERIFIER_nondet_char();
unsigned short __VERIFIER_nondet_ushort();
unsigned int __VERIFIER_nondet_uint();
int i;
unsigned short key;
char str[N];
char copyf[4];
pthread_mutex_t mutex;
void *client(void *arg)
{
  pthread_mutex_lock(&mutex);
  for(i = 0; (i < N && str[i] != '\0'); i++)
    str[i] = str[i] + key;
  pthread_mutex_unlock(&mutex);
  pthread_exit(0);
}
void *server(void *arg)
{
  pthread_mutex_lock(&mutex);
  for(i = 0; (i < N && str[i] != '\0'); i++) // the decrypted message
    str[i] = str[i] - key;
  pthread_mutex_unlock(&mutex);
  pthread_exit(0);
}
int main()
{
  key = __VERIFIER_nondet_ushort() % 26 + 1;
  pthread_t encryption, decryption;
  pthread_create(&decryption, 0, server, 0);
  pthread_create(&encryption, 0, client, 0);
  assert(pthread_detach(encryption) == 0);
  assert(pthread_detach(encryption) == 0);
  pthread_join(decryption, 0);
}
// Imported from ESBMC regression/esbmc-unix2/01_pthread67
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
