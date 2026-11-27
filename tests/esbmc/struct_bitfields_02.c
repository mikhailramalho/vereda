#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct str
{
  unsigned int a : 2;
  unsigned int b : 9;
  unsigned int c : 28;
} var;

int main()
{
  var.a = 3;
  var.b = 233;
  var.c = 1478;
  struct str *ptr;
  ptr = malloc(5);
  memcpy(ptr, &var, 5);
  if(ptr->a != 3)
  {
    free(ptr);
  }
  if(ptr->b != 233)
  {
    free(ptr);
  }
  if(ptr->c != 1478)
  {
    free(ptr);
  }
  free(ptr);
  return 0;
}
// Imported from ESBMC regression/esbmc/struct_bitfields_02
// RUN: --force-malloc-success --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: KNOWNBUG
// Vereda: verdict FAILED
