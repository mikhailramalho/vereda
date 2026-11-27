#include <assert.h>
#include <stddef.h>

struct S
{
  int i;
  char ch;
  int j;
};

int main(void)
{
  assert(__builtin_offsetof(struct S, i)==0);
  assert(__builtin_offsetof(struct S, ch)==4);
  assert(__builtin_offsetof(struct S, j)==8);
}
// Imported from ESBMC regression/cbmc/01_cbmc_offsetof1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
