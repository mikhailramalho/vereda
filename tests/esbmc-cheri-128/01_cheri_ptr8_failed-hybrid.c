#include <stdio.h>
#include <string.h>
#include <cheri/cheric.h>

char *test;
char *buffer = "hello";
char *secret = "secret";

int main(int argc, char **argv)
{
  char *__capability cap_ptr = cheri_ptr(test, 6);
  /* Overflow buffer, leaking the secret in a traditional system */
  for(int i = 0; i < strlen(buffer); i++)
  {
    printf("ptr[%d] = '%c'\n", i, cap_ptr[i]);
  }
  return 0;
}
// Imported from ESBMC regression/cheri-128/01_cheri_ptr8_failed-hybrid
// Needs: --cheri hybrid --big-endian
// RUN: --cheri hybrid --big-endian
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
