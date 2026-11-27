#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct str
{
  char a : 1;
  int b : 5;
} var;

int main()
{
  var.a = 0;
  var.b = 1;
  struct str *ptr;
  ptr = malloc(10);
  *ptr = var;
  printf("ptr->a = %d\n", ptr->a);
  printf("ptr->b = %d\n", ptr->b);
  assert(ptr->a == 0);
  assert(ptr->b == 1);
  if(ptr->a)
  {
    free(ptr);
  }
  if(!ptr->b)
  {
    free(ptr);
  }
  free(ptr);
  return 0;
}
// Imported from ESBMC regression/esbmc/struct_bitfields_03
// RUN: --force-malloc-success --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
